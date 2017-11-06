/**
  ******************************************************************************
  * @file    WiFiServerST.cpp
  * @author  Wi6labs
  * @brief   Server functions
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
#include "WiFiClientST.h"
#include "WiFiServerST.h"

/**
 * @brief  Constructor
   @param  port: port
 * @retval None
 */
WiFiServer::WiFiServer(uint16_t port)
{
  _port = port;
}

/**
 * @brief  Start server Connection
   @param  None
 * @retval None
 */
void WiFiServer::begin()
{
  int8_t sock;
  sock = DrvWiFi->getFreeSocket();
  if (sock != -1)
  {
    DrvWiFi->ES_WIFI_SetConnectionParam(sock, ES_WIFI_TCP_CONNECTION, _port);
    DrvWiFi->ES_WIFI_StartServerSingleConn(sock, COMM_SPI);
  }
}

WiFiClient WiFiServer::available(byte* status)
{
  static int cycle_server_down = 0;
  const int TH_SERVER_DOWN = 50;

  uint8_t sock = DrvWiFi->getCurrentSocket();
  WiFiClient client(sock);
  uint8_t _status = client.status();

  if (status != NULL)
  {
    *status = _status;
  }

  //server not in listen state, restart it
  if (cycle_server_down++ > TH_SERVER_DOWN)
  {
    DrvWiFi->ES_WIFI_SetConnectionParam(sock, ES_WIFI_TCP_CONNECTION, _port);
    DrvWiFi->ES_WIFI_StartServerSingleConn(sock, COMM_SPI);
    cycle_server_down = 0;
  }

  if (_status == SOCKET_BUSY)
  {
    return client;
  }
  return WiFiClient(255);
}

/**
 * @brief  Get server state
   @param  None
 * @retval None
 */
uint8_t WiFiServer::status() {
  return 1; // NOT SUPPORTED
}

/**
 * @brief  Write a single byte into the packet
   @param  data to write
 * @retval size of write data
 */
size_t WiFiServer::write(uint8_t b)
{
  return write(&b, 1);
}


/**
 * @brief  Write size bytes from buffer into all the active socket
   @param buffer : data to write
   @param size : size of data to write
 * @retval Number of active socket
 */
size_t WiFiServer::write(const uint8_t *buffer, size_t size)
{
  size_t n = 0;

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
  {
    if (DrvWiFi->getSocketState(sock) != SOCK_NOT_AVAIL)
    {
      WiFiClient client(sock);

      if (client.status() == SOCKET_BUSY)
      {
        n+=client.write(buffer, size);
      }
    }
  }
  return n;
}

/******************* (C) COPYRIGHT 2017 Wi6labs ***********END OF FILE********/
