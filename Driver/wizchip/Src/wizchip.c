#include "wizchip.h"
#include "Internet/DHCP/dhcp.h"
#include "Internet/DNS/dns.h"
#define LOG_DEFAULT_MODULE LOG_M_W5500
#include "loglib.h"
#include "spi.h"
#include "usart.h"
#include "tx_api.h"

#ifndef PP3V1_EHT_ON_GPIO_Port
#define PP3V1_EHT_ON_GPIO_Port ETH_ON_GPIO_Port
#endif

#ifndef PP3V1_EHT_ON_Pin
#define PP3V1_EHT_ON_Pin ETH_ON_Pin
#endif

static uint8_t dhcpBuff[2048];
static uint8_t IPFromDHCP[4];

extern SPI_HandleTypeDef ETH_SPI;

static TX_THREAD    ETH_DHCPThread;
static TX_TIMER     ETH_DHCPTimer;
static TX_MUTEX     ETH_Mutex;
static TX_SEMAPHORE ETH_Semaphore;

static void ETH_startDHCPTask(ULONG initial_input) {
    for (;;) {
        DHCP_run();
#if 0
        uint8_t ret = DHCP_run();
        LOG_DEBUG("DHCP_run() return status is %d.", ret);
#endif
        tx_thread_sleep(5000);
    }
}

void ETH_IPAssign(void) {
    LOG_INFO("IP has been assigned by DHCP.");
    getIPfromDHCP(IPFromDHCP);
    setSIPR(IPFromDHCP);
    LOG_INFO("The IP from DHCP is %d.%d.%d.%d.", IPFromDHCP[0], IPFromDHCP[1], IPFromDHCP[2], IPFromDHCP[3]);
    getGWfromDHCP(IPFromDHCP);
    setGAR(IPFromDHCP);
    LOG_INFO("The Gateway from DHCP is %d.%d.%d.%d.", IPFromDHCP[0], IPFromDHCP[1], IPFromDHCP[2], IPFromDHCP[3]);
    getSNfromDHCP(IPFromDHCP);
    setSUBR(IPFromDHCP);
    LOG_INFO("The Subnet Mask from DHCP is %d.%d.%d.%d.", IPFromDHCP[0], IPFromDHCP[1], IPFromDHCP[2], IPFromDHCP[3]);
}

void ETH_IPUpdate(void) {
    LOG_INFO("IP has been updated by DHCP.");
    getIPfromDHCP(IPFromDHCP);
    setSIPR(IPFromDHCP);
    LOG_INFO("The IP from DHCP is %d.%d.%d.%d.", IPFromDHCP[0], IPFromDHCP[1], IPFromDHCP[2], IPFromDHCP[3]);
    getGWfromDHCP(IPFromDHCP);
    setGAR(IPFromDHCP);
    LOG_INFO("The Gateway from DHCP is %d.%d.%d.%d.", IPFromDHCP[0], IPFromDHCP[1], IPFromDHCP[2], IPFromDHCP[3]);
    getSNfromDHCP(IPFromDHCP);
    setSUBR(IPFromDHCP);
    LOG_INFO("The Subnet Mask from DHCP is %d.%d.%d.%d.", IPFromDHCP[0], IPFromDHCP[1], IPFromDHCP[2], IPFromDHCP[3]);
}

void ETH_IPConflict(void) {
    LOG_INFO("An IP conflict has occured.");
}

static void ETH_dhcpTimerCallback(ULONG initial_input) {
    DHCP_time_handler();
}

void wizchipCriticalEnter(void) {
    __disable_irq();
}

void wizchipCriticalExit(void) {
    __enable_irq();
}

static void wizchipCSSelect(void) {
    HAL_GPIO_WritePin(ETH_NSS_GPIO_Port, ETH_NSS_Pin, GPIO_PIN_RESET);
}

static void wizchipCSDeselect(void) {
    HAL_GPIO_WritePin(ETH_NSS_GPIO_Port, ETH_NSS_Pin, GPIO_PIN_SET);
}

static uint8_t wizchipSPIReadByte(void) {
    uint8_t byte = 0;
    HAL_SPI_Receive(&ETH_SPI, &byte, 1, HAL_MAX_DELAY);
    return byte;
}

void wizchipSPIWriteByte(uint8_t wb) {
    HAL_SPI_Transmit(&ETH_SPI, &wb, 1, HAL_MAX_DELAY);
}

void wizchipSPIReadBurst(uint8_t *pBuf, uint16_t len) {
    HAL_SPI_Receive(&ETH_SPI, pBuf, len, HAL_MAX_DELAY);
}

void wizchipSPIWriteBurst(uint8_t *pBuf, uint16_t len) {
    HAL_SPI_Transmit(&ETH_SPI, pBuf, len, HAL_MAX_DELAY);
}

void wizchipTxRxCpltCallback() {
}

wiz_status_t wizChipInit(void *memoryPoolPtr) {
    HAL_GPIO_WritePin(ETH_RESET_GPIO_Port, ETH_RESET_Pin, GPIO_PIN_SET);
    tx_thread_sleep(100);
    HAL_GPIO_WritePin(PP3V1_EHT_ON_GPIO_Port, PP3V1_EHT_ON_Pin, GPIO_PIN_SET);
    tx_thread_sleep(100);
    tx_mutex_create(&ETH_Mutex, "Mutex Ethernet", 1);
    tx_semaphore_create(&ETH_Semaphore, "Ethernet Semaphore", 0);

    reg_wizchip_cris_cbfunc(wizchipCriticalEnter, wizchipCriticalExit);
    reg_wizchip_cs_cbfunc(wizchipCSSelect, wizchipCSDeselect);
    reg_wizchip_spi_cbfunc(wizchipSPIReadByte, wizchipSPIWriteByte);
    reg_wizchip_spiburst_cbfunc(wizchipSPIReadBurst, wizchipSPIWriteBurst);
    reg_dhcp_cbfunc(ETH_IPAssign, ETH_IPUpdate, ETH_IPConflict);
    DHCP_init(6, dhcpBuff);

    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *) memoryPoolPtr;
    CHAR         *ETH_DHCPStackPointer = NULL;
    if (tx_byte_allocate(byte_pool, (void **) &ETH_DHCPStackPointer, 512, TX_NO_WAIT) != TX_SUCCESS) {
        return WIZ_ERROR;
    }

    if (wizchip_init(NULL, NULL) != 0) {
        tx_mutex_delete(&ETH_Mutex);
        tx_semaphore_delete(&ETH_Semaphore);
        return WIZ_ERROR;
    }

    tx_timer_create(&ETH_DHCPTimer, "Ethernet DHCP Timer", ETH_dhcpTimerCallback, 1000, 1000, 1000, TX_AUTO_ACTIVATE);
    tx_thread_create(&ETH_DHCPThread, "Ethernet Thread DHCP", ETH_startDHCPTask, 0, ETH_DHCPStackPointer, 1024, 0, 0,
                     TX_NO_TIME_SLICE, TX_AUTO_START);

    return WIZ_OK;
}
