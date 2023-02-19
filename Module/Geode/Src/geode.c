/**
 * @file geode.c
 * @brief RFM point-to-multipoint protocol
 * @version 0.1
 * @date 2022-10-19
 *
 *  (c) 2022
 */

/* if undefined size passed to RFM_Tx thread will hang (no doi0 callback generated) */

/*
 * !todo remove buffer coruption on rfm data tx
 * yet we copy data as it will be corrupted on Tx
 */

/* !todecide mb remove size parameter from RFM_Rx & RFM_Tx and use configured size? */
/* !todo make randomisation of broadcast in slot (take random segment) */
/* !todo disconnect possibility & task kill */
/* !todo make flexible macros for setting parameters && claculating tim parameters */
/* !todo make bigger tim prescaler to reduce impact of crystal oscillator divergence */
/* !todo commands processing state machine (master/slave reaction/production on/of commands) for flexibility and easy
 * maintenance */

/* !todo master announce & slave listening
 *   mb make some api functions like:
 *   connect, disconnect (init deinit?) (tasks created on connecting and suspended on disconnecting)
 *   (start & end) listen and announce or make it announde for some time
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>

#include "geode.h"
#include "tx_api.h"

#define LOG_DEFAULT_MODULE LOG_M_RFM
#include "loglib.h"

#include <memory.h>

/* NOTE:
 * period consist of 131 slots (announcing & 2 masters & 128 slaves), each slot consist of few segments (1 packet time +
 * guard time) * x 2 master + 128 connections packets 18 bytes (with 16 bytes of data) !todecide each period ~1s or
 * 1+130*(18+preamble_len+syncword_len)*8/boudrate 2 master packets are treated as one (same slave ack) if we want cell
 * breathing we need packet sequence number (or packet window)
 */

#define BAUD_RATE       19200
#define DEVIATION       25000
#define START_FREQUENCY freqSet[0]
#define BANDWIDTH       RFM_BW100kHz
#define SYNC_WORD       DEFAULT_SYNC_WORD
#define SYNC_LENGTH     SYNC_WORD_LEN
#define PAYLOAD_LENGTH  PKT_SIZE
#define PREAMBLE_LENGTH 16

/* mb move those to special geodeConfig.h for every target */
#if defined(TARGET_HUB)
#define GOD_TIM    (htim7)
#define RFM_header "rfm66.h"
#define RFM_CONFIG                                                                                           \
    {                                                                                                        \
        .modulation = RFM_MOD_GFSK, .frequency = freqSet[0], .baud_rate = BAUD_RATE, .deviation = DEVIATION, \
        .bandwidth = BANDWIDTH, .sync_word = SYNC_WORD, .sync_length = SYNC_LENGTH, .output_power = 10,      \
        .payload_length = PAYLOAD_LENGTH, .preamble_length = PREAMBLE_LENGTH, .variable_pkt_length = 0,      \
        .crc_enable = 1, .crc_autoClearOff = 0, .crc_ibm = 0, .agc_auto_on = 1, .ocp_on = 0                  \
    }
#elif defined(TARGET_DEVBOARD)
#define GOD_TIM    (htim7)
#define RFM_header "rfm66a.h"
#define RFM_CONFIG                                                                                                   \
    {                                                                                                                \
        .frequency = freqSet[0], .deviation = 25000, .baud_rate = 19200, .preamble_len = PREAMBLE_LENGTH,            \
        .payload_len = PAYLOAD_LENGTH, .sync_len = SYNC_LENGTH, .sync_word = SYNC_WORD, .modulation = RFM_MOD_2GFSK, \
        .crc = CCITT_16                                                                                              \
    }
#else
#error "Current build target not supported"
#endif

#include RFM_header

#define RFM_WAIT_FOREVER TX_WAIT_FOREVER

#ifdef GEODE_DEBUG
#define EXT_DEBUG
#endif

#include "extm.h"

#include EXT_ll_utils_header

/* same as __HAL_TIM_SET_AUTORELOAD, but wihtout side effect, and not portable like __HAL_TIM_SetAutoreload xD */
#define TIM_SET_AUTORELOAD(_h, _v) ((_h)->Init.Period = (_h)->Instance->ARR = (_v))

/* NOTE:
 * to track time timer and task notifying are used.
 * timer fires when segment(or fragment) time gone and increases thread notify value
 * so flag to wait is equal to slot which numbered from first as 1 to last as 131 (PERIOD).
 * after PERIOD elapsed encountered flags must be cleared by task.
 */

#define MCD_COUNT (3) /* !todo try to remove adding this value to slave pos */

#define MAX_CONN_FGC       ((GOD_MAX_CONN + 31) / 32)
#define MAX_CONN_DWC       (4)
#define FRAME_SIZE         (2U)
#define FRAME_TO_SLOTS(_f) (FRAME_SIZE * (_f))
#define PERIOD             (FRAME_TO_SLOTS(GOD_MAX_CONN + MCD_COUNT))

#define MPDS_TO_SYN (10U)

#define GOD_TIM_POSFLAGS_MASK EXT_VALUE_MASK(PERIOD)

/* !todo calculate TIM settings depending on rfm settings */
/* one segment takes 0,0215s (46,5 pkts/s) */
#define PKT_TIM_LEN       (26000U)
#define PKT_GUARD_TIM_LEN (6400U)
/* !todo make it more flexible and calculate from set boudrate and so on */
#define PKT_TIM_CNT (PKT_TIM_LEN + PKT_GUARD_TIM_LEN)
/* 96000000 / 60 = 1600000 Hz. to ms prescaler: 1600 */
#define PKT_OFFSET(_n) (EXT_MS_TO_TICKS(PKT_TIM_CNT / 1600U * FRAME_TO_SLOTS(_n)))

#define PKT_GUARD_RAND_MAX 255

/* positions of control slots */
/* this not even a slot as there is no notify from tim, and delay is done by waiting period slot */
#define MABC_TIM_POS (FRAME_TO_SLOTS(0))
#define MHDC_TIM_POS (FRAME_TO_SLOTS(1))
#define MDFC_TIM_POS (FRAME_TO_SLOTS(2))

#define TX_OFFSET (6400 / 2)

#define GOD_Tx(_p, _l) (RFM_Tx(_p, _l))

#define PKT_SIZE      (18)
#define MAX_DATA_SIZE (PKT_SIZE - 2)
#define BUF_TX_SIZE   (200)
#define BUF_RX_SIZE   (200)
#define BUF_ANN_SIZE  (PKT_SIZE * 12)
#define STR_SIZE      (100)

/* ptr to buffer without side effect */
#define bufUsedS(_p, _size) (((_p)->tail <= (_p)->head) ? ((_p)->head - (_p)->tail) : (_size + (_p)->head - (_p)->tail))
#define bufFreeS(_p, _size) (((_p)->tail <= (_p)->head) ? (_size + (_p)->tail - (_p)->head) : ((_p)->tail - (_p)->head))
#define bufFEndS(_p, _size) (((_p)->tail <= (_p)->head) ? (_size - (_p)->head) : ((_p)->tail - (_p)->head))
#define bufUEndS(_p, _size) (((_p)->tail <= (_p)->head) ? ((_p)->head - (_p)->tail) : (_size - (_p)->tail))

#define DWB_INDX_MASK    (0x1FU)
#define DW_INDX_SHIFT    (5)
#define ARR_DW_INDX(_n)  ((_n) >> DW_INDX_SHIFT)
#define ARR_DWB_INDX(_n) ((_n) &DWB_INDX_MASK)
/* _p = pointer to array, _n = position */
#define ARR_BIT_CHECK(_p, _n)  (BIT_CHECK((_p)[ARR_DW_INDX(_n)], ARR_DWB_INDX(_n)))
#define ARR_BIT_SET(_p, _n)    (BIT_SET((_p)[ARR_DW_INDX(_n)], ARR_DWB_INDX(_n)))
#define ARR_BIT_TOGGLE(_p, _n) (BIT_TOGGLE((_p)[ARR_DW_INDX(_n)], ARR_DWB_INDX(_n)))
#define ARR_BIT_RESET(_p, _n)  (BIT_RESET((_p)[ARR_DW_INDX(_n)], ARR_DWB_INDX(_n)))

typedef enum {
    cmd_nop,
    cmd_syn,
    cmd_dcn,
    msf_cmd = 0x0F, /* also used by slave on connection */
    msf_sn = 1 << 4,
    msf_ack = 1 << 5,
    msf_reserved = 0xC0
} com_flags_t; /* add full flag? */

/* wtf with formater? */
#define COM_CMD_CHECK(_v, _c) (((_v) &msf_cmd) == (_c))
#define COM_CMD_SET(_v, _c)   (((_v) &= ~msf_cmd), (_v |= _c))

typedef enum {
    dff_len = 0x3F,
    df_reserved = 0xC0
} df_flags_t;

typedef struct {
    df_flags_t mf_dlen;
    uint8_t    data[MAX_DATA_SIZE];
} __packed dfragmend_t;

typedef struct {
    com_flags_t flags : 8;
    uint32_t    ack[MAX_CONN_DWC];
    uint8_t     slot; /* fin dst | 0xFF for all. unused yet */
} __packed mheader_t;

typedef struct {
    uint8_t     slot; /* packet destination || 0xFF broadcast. unused yet */
    dfragmend_t df;
} __packed mdf_t;

typedef struct {
    com_flags_t flags : 8; /* source of msg on putting it to rxbuf */
    dfragmend_t df;
} __packed sdf_t;

typedef struct {
    struct {
        uint16_t head;
        uint16_t tail;
        uint8_t  full;
        uint8_t  data[BUF_RX_SIZE];
    } *buf;
} bufRx_t;

typedef struct {
    uint16_t head;
    uint16_t tail;
    uint8_t  full;
    uint8_t  buf[BUF_TX_SIZE];
} bufTx_t;

extern TIM_HandleTypeDef GOD_TIM;

#define STACK_SIZE     1024
#define BYTE_POLL_SIZE STACK_SIZE

__ALIGN_BEGIN static uint8_t pool_buf[BYTE_POLL_SIZE] __ALIGN_END;

/* as we have only one task using this pool we can simply use all memory in buffer */
#if 0
static TX_BYTE_POOL pool;
static uint8_t *mem_ptr;
#else
#define mem_ptr pool_buf
#endif    // 0

static TX_MUTEX muxBufTx;

static TX_MUTEX muxReadMBC;

static TX_SEMAPHORE semBufTx;

static TX_SEMAPHORE semReadMBC;

/* semBufTx && semTIM can be united in event_flag_group (thus less memory will be used) */
static TX_SEMAPHORE semTIM;

#define ARR_EVENT_FLAG_SET(_efa, _n, _o) (tx_event_flags_set(&_efa[ARR_DW_INDX(_n)], 1U << ARR_DWB_INDX(_n), _o))
#define ARR_EVENT_FLAG_GET(_efa, _n, _o, _a, _w) \
    (tx_event_flags_get(&_efa[ARR_DW_INDX(_n)], 1U << ARR_DWB_INDX(_n), _o, _a, _w))
#define ARR_EVENT_FLAG_LOOKUP(_efa, _n) ((_efa[ARR_DW_INDX(_n)].tx_event_flags_group_current >> ARR_DWB_INDX(_n)) & 1U)

/* move those in buffer structs ? */
static TX_EVENT_FLAGS_GROUP god_ef_mux[MAX_CONN_FGC];
static TX_EVENT_FLAGS_GROUP god_ef_ntfy[MAX_CONN_FGC];

static TX_THREAD taskGEODE;

enum {
    STAT_CONNECT = 0x7F,
    STAT_DEINITED = 0x80
} static statFlags = STAT_DEINITED;

static uint8_t pkt_len = PKT_SIZE;

#define DEFAULT_SYNC_WORD \
    { 0xFC, 0xFC, 0xFC, 0xFC }
#define SYNC_WORD_LEN (4)

static const uint32_t freqSet[] = { 863500, 864300, 864700 };
static const uint8_t  dSyncWord[SYNC_WORD_LEN] = DEFAULT_SYNC_WORD;

#define freqN countof(freqSet)

#define FLAGS_OFFSET          (sizeof(sdf_t) - MAX_DATA_SIZE)
#define DATA_FLAGS_OFFSET     (FLAGS_OFFSET - sizeof(df_flags_t))
#define THREAD_FLAG_SLOT_MASK (GOD_MAX_CONN - 1)

static uint8_t   node;
static uint8_t  *str = NULL;
static uint8_t  *bufAnn = NULL;
static bufTx_t  *bufTx = NULL;
static bufRx_t **arrRx = NULL;

#if 0
void TaskScanGEODE(void *argument) {
    GOD_Scan(pdMS_TO_TICKS(atoi((char *) argument)));
    osThreadExit();
}
void god_scan(uint8_t argc, void **argv) {
    taskScanGEODEHandle = osThreadNew(TaskScanGEODE, argv[0], &taskScanGEODE_attributes);
}
void god_ann(uint8_t argc, void **argv) {
    GOD_Announce(pdMS_TO_TICKS(atoi(argv[0])), argv[1]);
}
#endif    // 0

static __unused god_stat_t createArr(god_node_t n) {
    if ((arrRx != NULL) || (bufTx != NULL)) {
        return GOD_ERROR;
    }

    node = n;
    str = malloc(STR_SIZE);
    bufTx = malloc(sizeof(bufTx_t));
    bufTx->full = bufTx->tail = bufTx->head = 0;

    arrRx = (bufRx_t **) malloc(n * sizeof(bufRx_t *));
    for (uint8_t i = 0; i < node; i++) {
        arrRx[i] = malloc(sizeof(bufRx_t));
        arrRx[i]->buf = NULL;
    }

    if (n == GOD_NODE_MASTER) {
        bufAnn = malloc(PKT_SIZE - (SYNC_WORD_LEN - 1));
        *bufAnn = 0;
    }
    return GOD_OK;
}

static __unused god_stat_t createBuf(uint8_t slot) {
    if (arrRx[slot]->buf != NULL) {
        return GOD_ERROR;
    }
    arrRx[slot]->buf = malloc(sizeof(*(arrRx[slot]->buf)));
    arrRx[slot]->buf->full = arrRx[slot]->buf->data[DATA_FLAGS_OFFSET] = arrRx[slot]->buf->tail =
        arrRx[slot]->buf->head = 0;
    return GOD_OK;
}

static __unused god_stat_t delArr() {
    if (arrRx == NULL) {
        return GOD_ERROR;
    }
    for (uint8_t i = 0; i < node; i++) {
        if (arrRx[i]->buf != NULL) {
            free(arrRx[i]->buf);
        }
        free(arrRx[i]);
    }
    free(arrRx);
    arrRx = NULL;
    free(bufTx);
    bufTx = NULL;
    if (bufAnn != NULL) {
        free(bufAnn);
        bufAnn = NULL;
    }
    free(str);
    str = NULL;
    return GOD_OK;
}

static __unused god_stat_t delBuf(uint8_t slot) {
    if (arrRx[slot]->buf == NULL) {
        return GOD_ERROR;
    }
    free(arrRx[slot]->buf);
    arrRx[slot]->buf = NULL;
    return GOD_OK;
}

god_stat_t GOD_DelAnnBuf() {
    if (bufAnn == NULL) {
        return GOD_ERROR;
    }
    free(bufAnn);
    bufAnn = NULL;
    return GOD_OK;
}

static __unused god_stat_t writeRxMsg(uint8_t *data) {
    uint8_t tmp, size, slot;
    slot = data[0];
    size = data[1] & dff_len;

    if ((arrRx[slot]->buf->full) || (size > bufFreeS(arrRx[slot]->buf, BUF_RX_SIZE))) {
        return GOD_BUF_FULL;
    }

    data += FLAGS_OFFSET;
    while (size) {
        tmp = ((size < bufFEndS(arrRx[slot]->buf, BUF_RX_SIZE)) ? (size) : (bufFEndS(arrRx[slot]->buf, BUF_RX_SIZE)));
        memcpy(arrRx[slot]->buf->data + arrRx[slot]->buf->head, data, tmp);
        arrRx[slot]->buf->head += tmp;
        data += tmp;
        size -= tmp;
        if (arrRx[slot]->buf->head == BUF_RX_SIZE) {
            arrRx[slot]->buf->head = 0;
        }
        if (arrRx[slot]->buf->head == arrRx[slot]->buf->tail) {
            arrRx[slot]->buf->full = 1;
        }
    }
    ARR_EVENT_FLAG_SET(god_ef_ntfy, slot, TX_OR);
    semReadMBC.tx_semaphore_count = slot;
    tx_semaphore_put(&semReadMBC);

    return GOD_OK;
}

static __unused uint8_t getTxMsg(uint8_t *buf) {
    uint8_t tmp, size;
    uint8_t full = bufTx->full; /* save status or disable context switch on putting semaphore */
    if ((bufTx->head == bufTx->tail) && !full) {
        return 0;
    }
    size = ((full) ? (PKT_SIZE - FLAGS_OFFSET) :
                     ((bufUsedS(bufTx, BUF_TX_SIZE) < (PKT_SIZE - FLAGS_OFFSET)) ? (bufUsedS(bufTx, BUF_TX_SIZE)) :
                                                                                   (PKT_SIZE - FLAGS_OFFSET)));
    buf[0] = size;
    buf++;
    while (size) {
        tmp = ((bufTx->full) ? ((size < (BUF_TX_SIZE - bufTx->tail)) ? (size) : (BUF_TX_SIZE - bufTx->tail)) :
                               ((size < bufUEndS(bufTx, BUF_TX_SIZE)) ? (size) : bufUEndS(bufTx, BUF_TX_SIZE)));
        memcpy(buf, bufTx->buf + bufTx->tail, tmp);
        bufTx->tail += tmp;
        size -= tmp;
        buf += tmp;
        if (bufTx->tail == BUF_TX_SIZE) {
            bufTx->tail = 0;
        }
        if ((bufTx->full) && (bufTx->tail != bufTx->head)) {
            bufTx->full = 0;
        }
    }
    if (full) {
        tx_semaphore_put(&semBufTx);
    }
    return PKT_SIZE;
}

static rfm_stat_t initRFM() {
    rfm_config_t cnfg = RFM_CONFIG;
    return RFM_Init() | RFM_Config(&cnfg);
}

/* yet it just linear */
uint32_t setNextFreq(uint8_t step) {
    static uint8_t i = 0;
    i += step;
    if (i >= freqN) {
        i %= freqN;
    }
    RFM_SetMode(RFM_MODE_STANDBY);
    RFM_SetFrequency(freqSet[i]);
    return freqSet[i];
}

static inline uint8_t getSlot(uint32_t sar[MAX_CONN_DWC]) {
    uint8_t i = 0, k;
    while (i < MAX_CONN_DWC && (k = clz(sar[i])) == 32) {
        ++i;
    }
    return (i << DW_INDX_SHIFT) | k; /* (i == MAX_CONN_DWC) ? ~0U : (i << DW_INDX_SHIFT) | (k & DWB_INDX_MASK); */
}

/* check for subset in ~sets. inline yet */
static inline uint8_t nsubset(uint32_t *na, uint32_t *nb, uint8_t size) {
    for (register uint8_t i = 0; i < size; ++i) {
        if ((na[i] | nb[i]) != na[i]) {
            return 0;
        }
    }
    return 1;
}

static int16_t tim_val;

static void waitTIM() {
    if (tim_val > 0) {
        tx_semaphore_get(&semTIM, TX_WAIT_FOREVER);
    }
}

void mTaskGEODE(uint32_t arg) {
    /* adjust master segments griding to others ? */
    const uint32_t mSyncDW = LL_GetUID_Word0();
    uint8_t        rep = 0, pos = 0;
    uint32_t       sAck[MAX_CONN_DWC];
    uint32_t       slots[MAX_CONN_DWC]; /* control on connection and disconnection */
    uint16_t       synPDcnt = MPDS_TO_SYN;

    mheader_t mh = {};
    mdf_t     mdf = {};
    sdf_t     sdf = {};
    uint8_t   buf[PKT_SIZE];

    srand(tx_time_get());
    const uint16_t pkt_time = PKT_TIM_CNT + ((uint64_t) rand() * PKT_GUARD_RAND_MAX) / RAND_MAX;

    memset(slots, ~0UL, sizeof(slots));
    memset(sAck, ~0UL, sizeof(sAck));

    mdf.slot = GOD_MBC;    // is not used yet

    memcpy(buf, &mSyncDW, sizeof(mSyncDW));
    RFM_SetSyncWord(buf, sizeof(mSyncDW));

    LOG_INFO("MASTER: started with %08x pkt_time %d", mSyncDW, pkt_time);

    TIM_SET_AUTORELOAD(&GOD_TIM, pkt_time);
    HAL_TIM_Base_Start_IT(&GOD_TIM);
    tim_val = 0;

    while (1) {
        /* add some randomization of translation slot segment */
        EXT_DLOG("MASTER: elapsed time %ld\n", DWT_CYCCNT / 9600, DWT_CYCCNT = 0);

        /* first slot for announcing */
        /* Tx our sync word + pos for new connection. we can announce multiple free slots */
        if (statFlags & STAT_CONNECT && (--statFlags, pos < GOD_MAX_CONN)) {
            EXT_DLOG("MASTER: announce");
            /* set announce syncword tx some data and return own syncword */
            memcpy(buf, dSyncWord, sizeof(dSyncWord)); /* get rid of this shit */
            RFM_SetSyncWord(buf, SYNC_WORD_LEN);
            buf[SYNC_WORD_LEN] = pos;
            *(uint16_t *) &buf[SYNC_WORD_LEN + 1] = pkt_time;
            memcpy(buf, &mSyncDW, sizeof(mSyncDW));
            memcpy(buf + (SYNC_WORD_LEN + 3), bufAnn, PKT_SIZE - (SYNC_WORD_LEN + 3));
            waitTIM();
            GOD_Tx(buf, pkt_len);
            memcpy(buf, &mSyncDW, sizeof(mSyncDW));
            RFM_SetSyncWord(buf, sizeof(mSyncDW));
        }

        /* some commands processing */
        if (!--synPDcnt) {
            /* !todo some delaying if another grand command is processed */
            synPDcnt = MPDS_TO_SYN;
            COM_CMD_SET(mh.flags, cmd_syn);
            rep |= 1;
            EXT_DLOG("MASTER: cmd_syn");
        }

        /* slots for master header */
        tim_val += MHDC_TIM_POS - MABC_TIM_POS;
        if (rep || (statFlags & STAT_CONNECT)) {
            memcpy(buf, &mh, PKT_SIZE); /* get rid of this shit */
            waitTIM();
            GOD_Tx(buf, pkt_len);
            EXT_DLOG("MASTER: tx header");
            COM_CMD_SET(mh.flags, cmd_nop); /* yet just clean it. l8r make state machine of commands processing */
            rep &= ~4U;
        }

        /* slot for data fragment */
        tim_val += MDFC_TIM_POS - MHDC_TIM_POS;
        if (rep & 2) {
            EXT_DLOG("MASTER: tx data");
            memcpy(buf, &mdf, PKT_SIZE); /* get rid of this shit */
            waitTIM();
            GOD_Tx(buf, pkt_len);
        }

        for (uint8_t i = 0; i < GOD_MAX_CONN; ++i) {
            tim_val += FRAME_TO_SLOTS(1);
            if ((ARR_BIT_CHECK(slots, i ^ DWB_INDX_MASK) && !((statFlags & STAT_CONNECT) && i == pos)) ||
                ((waitTIM()), RFM_Rx((uint8_t *) &sdf, &pkt_len, PKT_OFFSET(1)) != RFM_OK)) {
                continue;
            }
            rep |= 4;

            EXT_DLOG("MASTER: slave %d process", i);

            /* if we expecting connection listen slot that we announced */
            /* process connection on success stop announcing*/
            if (COM_CMD_CHECK(sdf.flags, msf_cmd)) {
                mh.flags ^= msf_ack;
                if (statFlags & STAT_CONNECT) {
                    /* process connection */
                    LOG_INFO("MASTER: slave %d connecting", i);
                    ARR_BIT_RESET(slots, i ^ DWB_INDX_MASK);
                    statFlags &= ~STAT_CONNECT;
                    continue;
                }
                /* second step in connection */
                if (!ARR_BIT_CHECK(slots, i ^ DWB_INDX_MASK) && (ARR_BIT_CHECK(mh.ack, i) ^ !!(sdf.flags & msf_sn))) {
                    createBuf(i);
                    /* mb change getSlot fn to remove those index inversions, and make random or max slaves dispensation
                     * (not in row as is) */
                    ARR_BIT_RESET(sAck, i ^ DWB_INDX_MASK);
                    pos = getSlot(slots);
                    LOG_INFO("MASTER: slave %d connected", i);
                    /* pass more info to ensure that it is ours node */
                } else {
                    /* if we get here and it already connected something happened to slave. reject him and warn user */
                    LOG_WARN("MASTER: slave %d missbehavior", i);
                    mh.flags ^= msf_ack;
                    ARR_BIT_SET(slots, i ^ DWB_INDX_MASK);
                }
                continue;
            }

            /* !todo commands processing */

            if (ARR_BIT_CHECK(sAck, i ^ DWB_INDX_MASK) && ((mh.flags & msf_sn) ^ ((sdf.flags & msf_ack) >> 1))) {
                ARR_BIT_RESET(sAck, i ^ DWB_INDX_MASK);
                EXT_DLOG("MASTER: slave %d got msg", i);
            }

            if (ARR_BIT_CHECK(mh.ack, i) == !!(sdf.flags & msf_sn)) {
                EXT_DLOG("MASTER: got data from slave %d", i);
                /* there is some new data */
                sdf.flags = i;
                writeRxMsg((uint8_t *) &sdf);
                /* do not ack it if buffer is full?
                 * but in this case slave will spam each period.
                 * special command for suspending tx can be added to prevent such situations
                 */

                ARR_BIT_TOGGLE(mh.ack, i);
            }
        }

        if (nsubset(slots, sAck, MAX_CONN_DWC) && ((rep &= ~3U), getTxMsg(((uint8_t *) &mdf) + 1))) {
            EXT_DLOG("MASTER: preparing new msg");
            rep |= 3;
            memset(sAck, ~0LU, sizeof(sAck));
            mh.flags ^= msf_sn;
        }
        setNextFreq(1);
        tim_val += FRAME_TO_SLOTS(1);
        waitTIM();
    }
}

/* do we need this as function */
static uint8_t sEnsureTx(uint8_t *buf, uint8_t *data, uint8_t pos) {
    memcpy(buf, data, pkt_len); /* get rid of this shit */
    tim_val += FRAME_TO_SLOTS(pos + MCD_COUNT) - MHDC_TIM_POS;
    waitTIM();
    GOD_Tx(buf, pkt_len);
    EXT_DLOG("tx header %d %d", pos, DWT_CYCCNT / 9600, DWT_CYCCNT = 0);

    setNextFreq(1);

    EXT_DLOG("listen master...");
    tim_val += FRAME_TO_SLOTS(GOD_MAX_CONN - pos) + MHDC_TIM_POS;
    waitTIM();

    /* !todo meaningful connection information */
    return RFM_Rx(buf, &pkt_len, PKT_OFFSET(1)) != RFM_OK ||
           /* add master decline possibility */
           (((mheader_t *) buf)->flags ^ ((sdf_t *) data)->flags) & msf_ack;
}

static inline uint8_t connect(uint8_t pos, sdf_t *sdf_ptr,
                              uint8_t *buf) { /* add some timeout? and return state to user? */
    EXT_DLOG("Trying connect to master...");

    COM_CMD_SET(sdf_ptr->flags, msf_cmd | msf_ack | msf_sn); /* mb add some data for connection */

    /* find translation, get first packet, establish connection and synchronization */
    while (RFM_Rx(buf, &pkt_len, PKT_OFFSET(PERIOD * freqN + 1)) == RFM_ERR_TIMEOUT) {
        EXT_DLOG("Failed to find translations");
        /* !todo give it finite tries? */
    }
    EXT_DLOG("Found translation");

    /* get masters header */
    while (setNextFreq(1), RFM_Rx(buf, &pkt_len, RFM_WAIT_FOREVER) != RFM_OK) {
    }

    HAL_TIM_Base_Start_IT(&GOD_TIM);
    __HAL_TIM_SET_COUNTER(&GOD_TIM, PKT_TIM_LEN + TX_OFFSET);
    tim_val = 0;    // write here slot passed in master header
    EXT_DLOG("TIM start");

    sdf_ptr->flags ^= (((mheader_t *) buf)->flags & msf_ack);

    while (sEnsureTx(buf, (uint8_t *) sdf_ptr, pos)) {
    }

    EXT_DLOG("Go ahead");
    if (ARR_BIT_CHECK(((mheader_t *) buf)->ack, pos)) {
        sdf_ptr->flags ^= msf_sn;
    }
    sdf_ptr->flags ^= msf_ack;

    while (sEnsureTx(buf, (uint8_t *) sdf_ptr, pos)) {
    }

    COM_CMD_SET(sdf_ptr->flags, cmd_nop);
    if (((mheader_t *) buf)->flags & msf_sn) {
        (sdf_ptr->flags &= ~msf_ack);
    } else {
        (sdf_ptr->flags |= msf_ack);
    }

    return 0; /*some return value? */
}

void sTaskGEODE(uint32_t arg) {
    const uint8_t pos = *((uint8_t *) arg + SYNC_WORD_LEN);
    uint8_t       rep = 0;    // for this statFlags can be used
    sdf_t         sdf = {};
    mheader_t     mh = {};
    mdf_t         mdf = {};
    uint8_t       buf[PKT_SIZE];

    memcpy(buf, (uint8_t *) arg, SYNC_WORD_LEN);
    RFM_SetSyncWord(buf, SYNC_WORD_LEN);

    TIM_SET_AUTORELOAD(&GOD_TIM, *(uint16_t *) (arg + SYNC_WORD_LEN + 1));

    connect(pos, &sdf, (uint8_t *) &mh);    // somehow notify user about progress?

    LOG_INFO("SLAVE: connected as %d to %08x", pos, *(uint32_t *) arg);

    free(bufAnn);
    bufAnn = NULL;

    /* what if when we connected master was tx some data. how we gonna ignore it */

    createBuf(0);

    while (1) {
        /* we can't do not listen if already got data */
        tim_val += MDFC_TIM_POS - MHDC_TIM_POS;
        if ((waitTIM(), RFM_Rx((uint8_t *) &mdf, &pkt_len, PKT_OFFSET(1)) == RFM_OK) &&
            ((rep = 1), (mh.flags & msf_sn) == ((sdf.flags & msf_ack) >> 1))) {
            EXT_DLOG("SLAVE: got data from master");
            /* there is some new data */
            sdf.flags ^= msf_ack;
            /* handle slot (compare it with own pos) */

            mdf.slot = 0; /* ? */
            writeRxMsg((uint8_t *) &mdf);
        }
        EXT_DLOG("SLAVE: elapsed time %ld\n", DWT_CYCCNT / 9600, DWT_CYCCNT = 0);

        /* listen master packets */
        if (ARR_BIT_CHECK(mh.ack, pos) ^ !!(sdf.flags & msf_sn) && ((rep = 0), getTxMsg(((uint8_t *) &sdf) + 1))) {
            /* there is some new data to tx */
            EXT_DLOG("SLAVE: preparing new msg");
            sdf.flags ^= msf_sn;
            rep = 1;
        }

        /* tx slot */
        /* add some randomization of translation in slot (random segment) */
        tim_val += FRAME_TO_SLOTS(pos) + MDFC_TIM_POS - MHDC_TIM_POS;
        if (rep) {
            EXT_DLOG("SLAVE: tx slot");
            memcpy(buf, &sdf, pkt_len); /* get rid of this shit */
            waitTIM();
            GOD_Tx(buf, pkt_len);
        }

        setNextFreq(1);

        /* rx slot */
        tim_val += FRAME_TO_SLOTS(GOD_MAX_CONN - pos) + MHDC_TIM_POS;
        if ((waitTIM()), RFM_Rx((uint8_t *) &mh, &pkt_len, PKT_OFFSET(1)) != RFM_OK) {
            continue;
        }

        EXT_DLOG("SLAVE: got master header");

        /* process commands */
        switch (mh.flags & msf_cmd) {
            case cmd_syn:
                EXT_DLOG("SLAVE: cmd_syn work out");
                HAL_TIM_GenerateEvent(&GOD_TIM, TIM_EVENTSOURCE_UPDATE);
                __HAL_TIM_SET_COUNTER(&GOD_TIM, PKT_TIM_LEN + TX_OFFSET);
                tim_val = 0; /* some another value that will be passed in master header? */
                /* mb add some response */
                break;
            default:
            case cmd_nop:
                break;
        }
    }
    /* bake dwidx and dwbidx in ARR_BIT_CHECK ? */
}

god_stat_t GOD_Announce(uint32_t time, uint8_t *text) {
    if (node != GOD_NODE_MASTER) {
        return GOD_ERROR;
    }
    if (text != NULL) {
        memcpy(bufAnn, text, PKT_SIZE - SYNC_WORD_LEN - 1);
    } else {
        bufAnn[0] = 0;
    }
    statFlags = ((time / PKT_OFFSET(PERIOD)) + 1) & STAT_CONNECT;
    return GOD_OK;
}

uint8_t *GOD_Scan(uint32_t time) {
    uint8_t  tmp, cnt = 0;
    uint8_t  buf[PKT_SIZE];
    uint32_t tim_cnt;
    memcpy(buf, dSyncWord, SYNC_WORD_LEN);
    RFM_SetSyncWord(buf, SYNC_WORD_LEN);
    if (bufAnn == NULL) {
        bufAnn = (uint8_t *) malloc(BUF_ANN_SIZE);
    }
    bufAnn[0] = 0;
    LOG_INFO("[SCAN] Start");
    time += tim_cnt = tx_time_get();
    while (tim_cnt < time) {
        if (RFM_Rx(buf, &tmp, time - tim_cnt) == RFM_OK) {
            if (bufAnn == NULL) {
                break;
            }
            for (uint8_t i = 0; i < cnt; ++i) {
                if (memcmp(buf, (bufAnn + 1) + (i * PKT_SIZE), PKT_SIZE) == 0) {
                    tmp = 0;
                    break;
                }
            }
            if (tmp != 0) {
                LOG_INFO("[SCAN] [%d] [%08x] %d %d msg: \"%s\" \n", cnt, *(uint32_t *) buf, *(uint8_t *) (buf + 4),
                         *(uint16_t *) (buf + 5), buf + 7);
                memcpy((bufAnn + 1) + (cnt * PKT_SIZE), buf, PKT_SIZE);
                bufAnn[0] = ++cnt;
            }
        } else {
            break;
        }
        tim_cnt = tx_time_get();
    }
    LOG_INFO("[SCAN] End");
    return bufAnn;
}

god_stat_t GOD_Init() {
    /* assert for mheader_t (or any other one) > PKT_SIZE */

    if (tx_mutex_create(&muxBufTx, NULL, TX_INHERIT) != TX_SUCCESS ||
        tx_mutex_create(&muxReadMBC, NULL, TX_INHERIT) != TX_SUCCESS ||
        tx_semaphore_create(&semReadMBC, NULL, 0) != TX_SUCCESS ||
        tx_semaphore_create(&semBufTx, NULL, 0) != TX_SUCCESS || tx_semaphore_create(&semTIM, NULL, 0) != TX_SUCCESS ||
        initRFM() != RFM_OK) {
        return GOD_ERROR;
    }

    for (uint8_t i = 0; i < MAX_CONN_FGC; ++i) {
        if (tx_event_flags_create(&god_ef_mux[i], NULL) != TX_SUCCESS ||
            ((tx_event_flags_set(&god_ef_mux[i], TX_WAIT_FOREVER, TX_OR)),
             (tx_event_flags_create(&god_ef_ntfy[i], NULL) != TX_SUCCESS))) {
            return GOD_ERROR;
        }
    }

#if 0
    tx_byte_pool_create(&pool, NULL, pool_buf, BYTE_POLL_SIZE);
    tx_byte_allocate(&pool, &mem_ptr, STACK_SIZE, TX_NO_WAIT);
#endif    // 0

    EXT_DINIT();

#if 0
    PARSER_AddCommand(god_scan, "rfm scan -t 0");
    PARSER_AddCommand(god_ann, "rfm ann -t 0 -m \"\"");
#endif    // 0

    statFlags &= ~STAT_DEINITED;

    return GOD_OK; /* normal return value? */
}

god_stat_t GOD_Start(god_node_t nt, void *args) {

    if (statFlags & STAT_DEINITED) {
        return GOD_ERROR;
    }

    createArr(nt);
    EXT_DLOG("Created buffers");

    /* clang-format off */
#ifdef EXT_DEBUG
    uint32_t create_status =
#endif
    /* allocate less stack size for slave? */
    tx_thread_create(&taskGEODE, NULL, nt == GOD_NODE_MASTER ? mTaskGEODE : sTaskGEODE, (ULONG) args, mem_ptr,
                     STACK_SIZE, 7, 7, TX_NO_TIME_SLICE, TX_AUTO_START);
    /* clang-format on */

    EXT_DLOG(create_status ? "Failed to create GEODE task" : "Created GEODE task");

    return GOD_OK;
}

god_stat_t GOD_Write(uint8_t *data, uint16_t size) {
    uint16_t tmp;
    if (size < 1) {
        return GOD_ERROR;
    }
    tx_mutex_get(&muxBufTx, TX_WAIT_FOREVER);
    do {
        if (bufTx->full) {
            tx_semaphore_get(&semBufTx, TX_WAIT_FOREVER);
        }
        tmp = (size < bufFEndS(bufTx, BUF_TX_SIZE) ? size : bufFEndS(bufTx, BUF_TX_SIZE));
        memcpy(bufTx->buf + bufTx->head, data, tmp);
        bufTx->head += tmp;
        data += tmp;
        size -= tmp;
        if (bufTx->head == BUF_TX_SIZE) {
            bufTx->head = 0;
        }
        if (bufTx->head == bufTx->tail) {
            bufTx->full = 1;
        }
    } while (size);
    tx_mutex_put(&muxBufTx);
    return GOD_OK;
}

#define bufUsedSF(_p, _size) (_p->full ? _size : bufUsedS(_p, _size))

/* !tocheck whether it actual !bug For some reason if thread that called this API have higher priority than taskGEODE it
 * works unpredictable (can't read if there is few data) */
god_stat_t GOD_Read(uint8_t slot, uint8_t *data, uint16_t size, god_mode_t mode) {
    uint32_t flags;
    uint8_t  status = GOD_OK;
    uint8_t  tmp = slot;

    /* setup logic */
    /* mb move GOD_MBC (read from any) sign to mode and use switch here */
    if (mode == GOD_NON_BLOCK) {
        // look for enough data
        if (slot == GOD_MBC) {
            tmp = 0;
            while (tmp < node && (!ARR_EVENT_FLAG_LOOKUP(god_ef_mux, tmp) || arrRx[tmp]->buf == NULL ||
                                  bufUsedSF(arrRx[tmp]->buf, BUF_RX_SIZE) < size)) {
                ++tmp;
            }
            if (tmp == node) {
                return GOD_BUF_EMPTY;
            }
        } else {
            if (!ARR_EVENT_FLAG_LOOKUP(god_ef_mux, slot) || bufUsedSF(arrRx[slot]->buf, BUF_RX_SIZE) < size) {
                return GOD_BUF_EMPTY;
            }
        }
    } else {
        if (slot == GOD_MBC) {
            // register TX_THREAD *cur_thread_ptr = tx_thread_identify();

            // tx_thread_priority_change(cur_thread_ptr, 0, &tmp);
            for (uint8_t i = 0; i < MAX_CONN_FGC; ++i) {
                tx_event_flags_get(&god_ef_mux[i], TX_WAIT_FOREVER, TX_AND_CLEAR, &flags, TX_WAIT_FOREVER);
            }
            // tx_thread_priority_change(cur_thread_ptr, tmp, &tmp);
        } else {
            ARR_EVENT_FLAG_GET(god_ef_mux, slot, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER);
        }
    }

    /* take logic */
    if (tmp == GOD_MBC) {
        tmp = 0;
        while (tmp < node && (arrRx[tmp]->buf == NULL || bufUsedSF(arrRx[tmp]->buf, BUF_RX_SIZE) < size)) {
            ++tmp;
        }
    }
    if (tmp == node) {
        do {
            tx_semaphore_get(&semReadMBC, TX_WAIT_FOREVER);
            status = tmp = semReadMBC.tx_semaphore_count;
            semReadMBC.tx_semaphore_count = 0;
        } while (bufUsedSF(arrRx[tmp]->buf, BUF_RX_SIZE) < size);
    }

    do {

        if (arrRx[tmp]->buf == NULL || (!arrRx[tmp]->buf->full && arrRx[tmp]->buf->head == arrRx[tmp]->buf->tail)) {
            ARR_EVENT_FLAG_GET(god_ef_ntfy, tmp, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER);
        }

        uint16_t len =
            ((arrRx[tmp]->buf->full) ?
                 ((size < BUF_RX_SIZE) ? (size) : (BUF_RX_SIZE)) :
                 ((size < bufUsedS(arrRx[tmp]->buf, BUF_RX_SIZE)) ? size : bufUsedS(arrRx[tmp]->buf, BUF_RX_SIZE)));

        do {
            uint16_t write_len =
                ((arrRx[tmp]->buf->full) ?
                     ((len < (BUF_RX_SIZE - arrRx[tmp]->buf->tail)) ? (len) : (BUF_RX_SIZE - arrRx[tmp]->buf->tail)) :
                     ((len < bufUEndS(arrRx[tmp]->buf, BUF_RX_SIZE)) ? len : bufUEndS(arrRx[tmp]->buf, BUF_RX_SIZE)));
            memcpy(data, arrRx[tmp]->buf->data + arrRx[tmp]->buf->tail, write_len);
            arrRx[tmp]->buf->tail += write_len;
            data += write_len;
            size -= write_len;
            len -= write_len;
            if (arrRx[tmp]->buf->tail == BUF_RX_SIZE) {
                arrRx[tmp]->buf->tail = 0;
            }
            if ((arrRx[tmp]->buf->tail != arrRx[tmp]->buf->head) && arrRx[tmp]->buf->full) {
                arrRx[tmp]->buf->full = 0;
            }
        } while (len); /* done at max 2 times */

    } while (size);

    /* unsetup logic */
    if (mode == GOD_BLOCK) {
        if (slot == GOD_MBC) {
            for (uint8_t i = 0; i < MAX_CONN_FGC; ++i) {
                tx_event_flags_set(&god_ef_mux[i], TX_WAIT_FOREVER, TX_OR);
            }
        } else {
            ARR_EVENT_FLAG_SET(god_ef_mux, slot, TX_OR);
        }
    }

    return status;
}

god_stat_t GOD_Printf(char *data, ...) {
    god_stat_t stat = GOD_OK;
    tx_mutex_get(&muxBufTx, TX_WAIT_FOREVER);
    va_list arg;
    va_start(arg, data);
    vsnprintf((char *) str, STR_SIZE, data, arg);
    va_end(arg);
    stat = GOD_Write(str, strnlen((char *) str, STR_SIZE) + 1);
    tx_mutex_put(&muxBufTx);
    return stat;
}

void GOD_TIM_Callback(TIM_HandleTypeDef *htim) {
    if (htim == &GOD_TIM) {
        if (!--tim_val) {
            tx_semaphore_put(&semTIM);
        }
    }
}
