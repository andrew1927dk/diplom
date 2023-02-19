/**
 * @file ethernet_adapter.h
 * @brief An ethernet adapter header
 * @version 0.1
 * @date 2022-10-12
 *
 *  (c) 2022
 */

#ifndef ETHERNET_ADAPTER_H
#define ETHERNET_ADAPTER_H

#include "web_adapter.h"

/**
 * @brief Open socket for ethernet connection.
 * @param socketNumber Socket number.
 * @param mode Protocol type to operate such as TCP, UDP and MACRAW.
 * @param srcPort Port number to be bined.
 * @param flags Socket flags as \ref SF_ETHER_OWN, \ref SF_IGMP_VER2, \ref SF_TCP_NODELAY, \ref SF_MULTI_ENABLE, \ref
 * SF_IO_NONBLOCK and so on. Valid flags only in W5500 : @ref SF_BROAD_BLOCK, @ref SF_MULTI_BLOCK, @ref SF_IPv6_BLOCK,
 * and @ref SF_UNI_BLOCK.
 * @return @b Success : The socket number @b 'sn' passed as parameter
 *         @b Fail    :  @ref SOCKERR_SOCKNUM     - Invalid socket number
 *                       @ref SOCKERR_SOCKMODE    - Not support socket mode as TCP, UDP, and so on.
 *                       @ref SOCKERR_SOCKFLAG    - Invaild socket flag.
 */
web_status_t ETH_SOCK_Open(uint8_t socketNumber, uint8_t mode, uint16_t srcPort, uint8_t flags);

/**
 * @brief Close socket for ethernet connection.
 * @param socketNumber Socket number.
 * @return @b Success : @ref SOCK_OK \n
 *         @b Fail    : @ref SOCKERR_SOCKNUM - Invalid socket number
 */
web_status_t ETH_SOCK_Close(uint8_t socketNumber);

/**
 * @brief Listen to a connection request from a client.
 * @param socketNumber Socket number.
 * @return @b Success : @ref SOCK_OK \n
 *         @b Fail    :\n @ref SOCKERR_SOCKINIT   - Socket is not initialized \n
 *                        @ref SOCKERR_SOCKCLOSED - Socket closed unexpectedly.
 */
web_status_t ETH_SOCK_Listen(uint8_t socketNumber);

/**
 * @brief Try to connect a server via an ethernet connection.
 * @param socketNumber Socket number.
 * @param destAddress Pointer variable of destination IP address. It should be allocated 4 bytes.
 * @param destPort Destination port number.
 * @return @b Success : @ref SOCK_OK \n
 * @b Fail    :\n @ref SOCKERR_SOCKNUM   - Invalid socket number\n
 *                @ref SOCKERR_SOCKMODE  - Invalid socket mode\n
 *                @ref SOCKERR_SOCKINIT  - Socket is not initialized\n
 *                @ref SOCKERR_IPINVALID - Wrong server IP address\n
 *                @ref SOCKERR_PORTZERO  - Server port zero\n
 *                @ref SOCKERR_TIMEOUT   - Timeout occurred during request connection\n
 *                @ref SOCK_BUSY         - In non-block io mode, it returned immediately\n
 */
web_status_t ETH_SOCK_Connect(uint8_t socketNumber, uint8_t *destAddress, uint16_t destPort);

/**
 * @brief Try to disconnect a connection socket.
 * @param socketNumber Socket number.
 * @return @b Success :   @ref SOCK_OK \n
 *         @b Fail    :\n @ref SOCKERR_SOCKNUM  - Invalid socket number \n
 *                        @ref SOCKERR_SOCKMODE - Invalid operation in the socket \n
 *                        @ref SOCKERR_TIMEOUT  - Timeout occurred \n
 *                        @ref SOCK_BUSY        - Socket is busy.
 */
web_status_t ETH_SOCK_Disconnect(uint8_t socketNumber);

/**
 * @brief	Send data to the connected peer in TCP socket via an ethernet connection.
 * @param socketNumber Socket number.
 * @param buffer Pointer buffer containing data to be sent.
 * @param len The byte length of data in buf.
 * @return	@b Success : The sent data size \n
 *          @b Fail    : \n @ref SOCKERR_SOCKSTATUS - Invalid socket status for socket operation \n
 *                          @ref SOCKERR_TIMEOUT    - Timeout occurred \n
 *                          @ref SOCKERR_SOCKMODE 	- Invalid operation in the socket \n
 *                          @ref SOCKERR_SOCKNUM    - Invalid socket number \n
 *                          @ref SOCKERR_DATALEN    - zero data length \n
 *                          @ref SOCK_BUSY          - Socket is busy.
 */
web_status_t ETH_SOCK_Send(uint8_t socketNumber, uint8_t *buf, uint16_t len);

/**
 * @brief	Receive data from the connected peer via an ethernet connection.
 * @param socketNumber  Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param buffer Pointer buffer to read incoming data.
 * @param maxLen The max data length of data in buf.
 * @return	@b Success : The real received data size \n
 *          @b Fail    :\n
 *                     @ref SOCKERR_SOCKSTATUS - Invalid socket status for socket operation \n
 *                     @ref SOCKERR_SOCKMODE   - Invalid operation in the socket \n
 *                     @ref SOCKERR_SOCKNUM    - Invalid socket number \n
 *                     @ref SOCKERR_DATALEN    - zero data length \n
 *                     @ref SOCK_BUSY          - Socket is busy.
 */
web_status_t ETH_SOCK_Recv(uint8_t socketNumber, uint8_t *buf, uint16_t maxLen);

/**
 * @brief	Sends datagram to the peer with destination IP address and port number passed as parameter via an ethernet
 * connection.
 * @param socketNumber    Socket number. It should be <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>.
 * @param buf   Pointer buffer to send outgoing data.
 * @param len   The byte length of data in buf.
 * @param destAddress  Pointer variable of destination IP address. It should be allocated 4 bytes.
 * @param destPort  Destination port number.
 * @return @b Success : The sent data size \n
 *         @b Fail    :\n @ref SOCKERR_SOCKNUM     - Invalid socket number \n
 *                        @ref SOCKERR_SOCKMODE    - Invalid operation in the socket \n
 *                        @ref SOCKERR_SOCKSTATUS  - Invalid socket status for socket operation \n
 *                        @ref SOCKERR_DATALEN     - zero data length \n
 *                        @ref SOCKERR_IPINVALID   - Wrong server IP address\n
 *                        @ref SOCKERR_PORTZERO    - Server port zero\n
 *                        @ref SOCKERR_SOCKCLOSED  - Socket unexpectedly closed \n
 *                        @ref SOCKERR_TIMEOUT     - Timeout occurred \n
 *                        @ref SOCK_BUSY           - Socket is busy.
 */
web_status_t ETH_SOCK_SendTo(uint8_t socketNumber, uint8_t *buf, uint16_t len, uint8_t *destAddress, uint16_t destPort);

/**
 * @brief Receive datagram of UDP or MACRAW via an ethernet connection.
 * @param socketNumber   Socket number.
 * @param buffer  Pointer buffer to read incoming data.
 * @param maxLen  The max data length of data in buf.
 *             When the received packet size <= len, receives data as packet sized.
 *             When others, receives data as len.
 * @param senderAddress Pointer variable of destination IP address. It should be allocated 4 bytes.
 *             It is valid only when the first call recvfrom for receiving the packet.
 *             When it is valid, @ref  packinfo[7] should be set as '1' after call @ref getsockopt(sn, SO_PACKINFO,
 * &packinfo).
 * @param senderPort Pointer variable of destination port number.
 *             It is valid only when the first call recvform for receiving the packet.
 *             When it is valid, @ref  packinfo[7] should be set as '1' after call @ref getsockopt(sn, SO_PACKINFO,
 * &packinfo).
 * @return	@b Success : This function return real received data size for success.\n
 *          @b Fail    : @ref SOCKERR_DATALEN    - zero data length \n
 *                       @ref SOCKERR_SOCKMODE   - Invalid operation in the socket \n
 *                       @ref SOCKERR_SOCKNUM    - Invalid socket number \n
 *                       @ref SOCKBUSY           - Socket is busy.
 */
web_status_t ETH_SOCK_RecvFrom(uint8_t socketNumber, uint8_t *buf, uint16_t maxLen, uint8_t *senderAddress,
                               uint16_t senderPort);

/**
 * @brief Initialize ethernet adapter
 * @return	(webInterface_t) - Ethernet web adapter configuration structure
 */
web_status_t ETH_AdapterInit(void *memoryPointer);

webInterface_t *ETH_GetAdapter(void);

/**
 * @brief Deinitialize ethernet adapter
 * @param adap Pointer to ethernet adapter
 * @return None
 */
void ETH_AdapterDeInit(void);

#endif    // ETHERNET_ADAPTER_H
