/**
  ******************************************************************************
  * @file    WiFiUdpST.cpp
  * @author  Wi6labs
  * @brief   Udp functions
  ******************************************************************************
  --------------------------------------------------------------------------

              j]_                   .___
._________    ]0Mm                .=]MM]=
M]MM]MM]M]1  jMM]P               d]-' NM]i
-~-~   4MM1  d]M]1              d]'   jM]'
       j]MT .]M]01       d],  .M]'    d]#
       d]M1 jM4M]1  .,  d]MM  d]I    .]M'
       ]0]  M/j]0(  d]L NM]f d]P     jM-
       M]M .]I]0M  _]MMi -' .]M'
       M]0 jM MM]  jM-M>   .]M/
       ]0F MT ]M]  M>      d]M1        .,
      j0MT.]' M]M j]1 .mm .]MM ._d]_,   J,
      jM]1jM  ]01 =] .]M/ jM]Fd]M]MM]   .'
      j]M1#T .M]1.]1 jM]' M]0M/^ "M]MT  j         .",    .__,  _,-_
      jMM\]' J]01jM  M]M .]0]P    ]0]1  i         1 1   .'  j .'  "1
      j]MJ]  jM]1]P .]M1 jMMP     MM]1  I        J  t   1   j J    '
      =M]dT  jM]q0' dM]  M]MT     ]MM  j        j   j  j    J 1
      ]M]M`  j]0j#  ]MF  ]M]'    .M]P  J       .'   j  J  .J  4_,
      M]0M   =MM]1 .M]'  MM]     jM](  1       r    j  1  --,   "!
      ]0MT   ]M]M  jM@   ]M]     M]P  j       J     j j     4     1
      MM]'   M]0P  j]1  .M]M    j]M'  J      j'     ",?     j     1
     _]M]    M]0`  jM1 .MNMM,  .]M'   1     .'       11     1    j'
     jM]1   jM]@   j]L_]'?M]M__MP'    \     J        1G    J    .'
     j]0(   jM]1   "M]P'  "N]M/-      "L__J L________'?L__- *__,'
     "-'    "--

----------------------------------------------------------------------------

Copyright (c) <2017>, <Wi6labs>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the wi6labs nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL WI6LABS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


-----------------------------------------------------------------------------*/

#include <string.h>
#include "wifi_drv.h"

#include "WiFiST.h"
#include "WiFiUdpST.h"

/* Constructor */
WiFiUDP::WiFiUDP() : _sock(NO_SOCKET_AVAIL) {}

/**
 * @brief  Initialize start listening on specified port
 * @param  port : port to listen
 * @retval Returns 1 if successful, 0 if there are no sockets available to use
 */
uint8_t WiFiUDP::begin(uint16_t port) {

  int8_t sock = DrvWiFi->getFreeSocket(); // get next free socket
  if (sock != -1)
  {
    // Set connection parameter and start server
    DrvWiFi->ES_WIFI_SetConnectionParam(sock, ES_WIFI_UDP_CONNECTION, port);
    DrvWiFi->ES_WIFI_StartServerSingleConn(sock, COMM_SPI);
    _sock = sock;
    _port = port;
    return 1;
  }
  return 0;
}

/**
 * @brief  Finish with the UDP Socket
 * @param  None
 * @retval None
 */
void WiFiUDP::stop()
{
  if (_sock != NO_SOCKET_AVAIL)
  {
    DrvWiFi->ES_WIFI_StopServerSingleConn(_sock);
    _sock = NO_SOCKET_AVAIL;
  }
}

/**
 * @brief  Start building up a packet to send to the remote host
 * @param  ip : ip to which to send the packet
 * @param  port : port to which to send the packet
 * @retval 1 if successful, 0 if there was a problem with the supplied IP address or port
 */
int WiFiUDP::beginPacket(IPAddress ip, uint16_t port)
{
  int8_t sock;
  if (_sock == NO_SOCKET_AVAIL)
  {
    sock = DrvWiFi->getFreeSocket(); // get next free socket
    if(sock != -1)
    {
      _sock = sock;
    }
  }
  if (_sock != NO_SOCKET_AVAIL)
  {
    // set connection parameter and start client connection
    DrvWiFi->ES_WIFI_SetConnectionParam(_sock, ES_WIFI_UDP_CONNECTION, port, ip);
    DrvWiFi->ES_WIFI_StartClientConnection(_sock);
    return 1;
  }
  return 0;
}

/**
 * @brief  Start building up a packet to send to the remote host
 * @param  host : host to which to send the packet
 * @param  port : port to which to send the packet
 * @retval 1 if successful, 0 if there was a problem with the supplied IP address or port
 */
int WiFiUDP::beginPacket(const char *host, uint16_t port)
{
  IPAddress remote_addr; // IP address of the host

  DrvWiFi->ES_WIFI_DNS_LookUp(host, &remote_addr);
  return beginPacket(remote_addr, port);
}

/**
 * @brief  Finish off this packet and send it
 * @param  None
 * @retval 1 if the packet was sent successfully, 0 if there was an error
 * @Note Function not supported, always return 1
 */
int WiFiUDP::endPacket()
{
  /***************************************************************************/
  /*                               NOT SUPPORTED                             */
  /* The current device directly send data when there are write in the socket*/
  /***************************************************************************/
  return 1;
}

/**
 * @brief  Write size bytes from buffer into the packet
 * @param  buffer: data to write
 * @param  size: size of data to write
 * @retval size of write data
 */
size_t WiFiUDP::write(const uint8_t *buffer, size_t size)
{
  uint16_t SentLen = 0; // number of data really send
  uint8_t *temp = (uint8_t*)buffer;

  DrvWiFi->ES_WIFI_SendResp(_sock, temp, size, &SentLen, WIFI_TIMEOUT);

  return SentLen;
}

/**
 * @brief  Write a single byte into the packet
 * @param  byte : data to write
 * @retval size of write data
 */
size_t WiFiUDP::write(uint8_t byte)
{
  return write(&byte, 1);
}

/**
 * @brief  Return number of bytes available in the current packet
 * @param  None
 * @retval number of bytes available in the current packet
 * @Note Function not supported, always return 0
 */
int WiFiUDP::available() {
  /***************************************************************************/
  /*                               NOT SUPPORTED                             */
  /* This functionality doesn't exist in the current device.                 */
  /***************************************************************************/
  return 0;
}

/**
 * @brief  Return number of bytes available in the current packet
 * @param  None
 * @retval number of bytes available in the current packet
 */
int WiFiUDP::parsePacket()
{
  return available();
}

/**
 * @brief  Read a single byte from the current packet
 * @param  None
 * @retval the read byte
 */
int WiFiUDP::read()
{
  uint8_t b = 0; // data received
  uint16_t RecLen = 0; // Number of data received

  DrvWiFi->ES_WIFI_ReceiveData(_sock, &b, 1, &RecLen, WIFI_TIMEOUT);
  return b;
}

/**
 * @brief  Read up to len bytes from the current packet and place them into buffer
 * @param  buffer : Where to place read data
 * @param  len : length of data to read
 * @retval Returns the number of bytes read, or 0 if none are available
 */
int WiFiUDP::read(unsigned char* buffer, size_t len)
{
  uint16_t RecLen = 0; // Number of data received

  DrvWiFi->ES_WIFI_ReceiveData(_sock, buffer, len, &RecLen, WIFI_TIMEOUT);
  if (RecLen < len)
  {
    buffer[RecLen] = '\0'; // string end
  }
  return RecLen;
}

/**
 * @brief  Return the next byte from the current packet without moving on to the next byte
 * @param  None
 * @retval next byte from the current packet without moving on to the next byte
 * @Note Not supported
 */
int WiFiUDP::peek()
{
  /***************************************************************************/
  /*                               NOT SUPPORTED                             */
  /* This functionality doesn't exist in the current device.                 */
  /***************************************************************************/
  return 0;
}

/**
 * @brief  Finish reading the current packet
 * @param  None
 * @retval None
 * @Note Function not supported
 */
void WiFiUDP::flush()
{
  /***************************************************************************/
  /*                               NOT SUPPORTED                             */
  /* Not supported in previous device, still not supported                   */
  /***************************************************************************/
}

/**
 * @brief  Return the IP address of the host who sent the current incoming packet
   @param  None
 * @retval the IP address of the host who sent the current incoming packet
 */
IPAddress  WiFiUDP::remoteIP()
{
  uint8_t remoteIp[4] = {0};
  uint16_t remotePort;

  DrvWiFi->ES_WIFI_getRemoteData(_sock, remoteIp, &remotePort);
  IPAddress ip(remoteIp);
  return ip;
}

/**
 * @brief  Return the port of the host who sent the current incoming packet
   @param  None
 * @retval the port of the host who sent the current incoming packet
 */
uint16_t  WiFiUDP::remotePort()
{
  uint8_t remoteIp[4] = {0};
  uint16_t remotePort = 0;

  DrvWiFi->ES_WIFI_getRemoteData(_sock, remoteIp, &remotePort);
  return remotePort;
}

/******************* (C) COPYRIGHT 2017 Wi6labs ***********END OF FILE********/
