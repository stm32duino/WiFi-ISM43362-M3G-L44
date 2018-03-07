/**
  ******************************************************************************
  * @file    WiFiClientST.cpp
  * @author  Wi6labs
  * @brief   Client functions
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

#include "WiFiST.h"
#include "WiFiClientST.h"
#include "wifi_drv.h"

/**
 * @brief  Constructor
 * @param  None
 * @retval None
 */
WiFiClient::WiFiClient() : _sock(SOCK_NOT_AVAIL) {
}

/**
 * @brief  Constructor
 * @param  sock: socket to use
 * @retval None
 */
WiFiClient::WiFiClient(uint8_t sock) : _sock(sock) {
}

 /**
 * @brief  Start building up a packet to send to the remote host
 * @param  ip : IP to which to send the packet
 * @param  port : port to which to send the packet
 * @retval 1 if successful, 0 if there was a problem with the supplied IP address or port
 */
int WiFiClient::connect(IPAddress ip, uint16_t port)
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
    // set connection parameter and start client
    DrvWiFi->ES_WIFI_SetConnectionParam(_sock, ES_WIFI_TCP_CONNECTION, port, ip);
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
int WiFiClient::connect(const char* host, uint16_t port)
{
  IPAddress remote_addr; // IP address of the host

  DrvWiFi->ES_WIFI_DNS_LookUp(host, &remote_addr);
  return connect(remote_addr, port);
}

/**
 * @brief  Write size bytes from buffer into the packet
 * @param  buf : data to write
 * @param  size : size of data to write
 * @retval size of write data
 */
size_t WiFiClient::write(const uint8_t *buf, size_t size)
{
  uint16_t SentLen = 0; // number of data really send
  uint8_t *temp = (uint8_t*)buf;

  DrvWiFi->ES_WIFI_SendResp(_sock, temp, size, &SentLen, WIFI_TIMEOUT);
  return SentLen;
}

/**
 * @brief  Write a single byte into the packet
 * @param  b : data to write
 * @retval size of write data
 */
size_t WiFiClient::write(uint8_t b)
{
  return write(&b, 1);
}

/**
 * @brief  Return number of bytes available in the current packet
 * @param  None
 * @retval number of bytes available in the current packet
 * @Note Function not supported , always return 1.
 */
int WiFiClient::available()
{
  /***************************************************************************/
  /*                               NOT SUPPORTED                             */
  /* This functionality doesn't exist in the current device.                 */
  /***************************************************************************/
  return 1;
}

/**
 * @brief  Read a single byte from the current packet
 * @param  None
 * @retval the read byte
 */
int WiFiClient::read()
{
  uint8_t b = '\0'; // data received
  uint16_t RecLen = 0; // number of data received
  DrvWiFi->ES_WIFI_ReceiveData(_sock, &b, 1, &RecLen, WIFI_TIMEOUT);
  return b;
}

/**
 * @brief  Read up to size bytes from the current packet and place them into buffer
 * @param  buffer : Where to place read data
 * @param  size : length of data to read
 * @retval Returns the number of bytes read, or 0 if none are available
 */
int WiFiClient::read(uint8_t* buf, size_t size)
{
  uint16_t RecLen = 0; // number of data received

  DrvWiFi->ES_WIFI_ReceiveData(_sock, buf, size, &RecLen, WIFI_TIMEOUT);
  if (RecLen < size)
  {
    buf[RecLen] = '\0'; // string end
  }
  return RecLen;
}

/**
 * @brief  Return the next byte from the current packet without moving on to the next byte
 * @param  None
 * @retval next byte from the current packet without moving on to the next byte
 * @Note Function not supported , always return 0.
 */
int WiFiClient::peek()
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
void WiFiClient::flush() {
  /***************************************************************************/
  /*                               NOT SUPPORTED                             */
  /* Not supported in previous device, still not supported                   */
  /***************************************************************************/
}

/**
 * @brief  Close the client connection
 * @param  None
 * @retval None
 */
void WiFiClient::stop()
{
  if (_sock == NO_SOCKET_AVAIL)
  {
    return;
  }
  DrvWiFi->ES_WIFI_StopServerSingleConn(_sock);
  _sock = NO_SOCKET_AVAIL;
}

/**
 * @brief  Get connection state
   @param  None
 * @retval Socket state, FREE or BUSY
 */
uint8_t WiFiClient::connected()
{
  if (_sock == 255)
  {
    return 0;
  }
  else
  {
    return status();
  }
}

/**
 * @brief  Get socket state
   @param  None
 * @retval socket state
 */
uint8_t WiFiClient::status() {
  if (_sock == 255)
  {
    return SOCKET_FREE;
  }
  else
  {
    return DrvWiFi->getSocketState(_sock);
  }
}

/**
  * @brief  .
  * @param  None
  * @retval None.
  */
WiFiClient::operator bool() {
  return _sock != 255;
}

/**
  * @brief  Get the first socket number free.
  * @param  None
  * @retval socket number or -1 if no available.
  */
// Private Methods
uint8_t WiFiClient::getFirstSocket()
{
  return DrvWiFi->getFreeSocket();
}

/******************* (C) COPYRIGHT 2017 Wi6labs ***********END OF FILE********/

