/**
  ******************************************************************************
  * @file    WiFiServerST.cpp
  * @author  Wi6labs
  * @brief   Server functions
  ******************************************************************************
  * Copyright (c) <2017>, <Wi6labs>
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *     * Redistributions of source code must retain the above copyright
  *       notice, this list of conditions and the following disclaimer.
  *     * Redistributions in binary form must reproduce the above copyright
  *       notice, this list of conditions and the following disclaimer in the
  *       documentation and/or other materials provided with the distribution.
  *     * Neither the name of the wi6labs nor the
  *       names of its contributors may be used to endorse or promote products
  *       derived from this software without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL WI6LABS BE LIABLE FOR ANY
  * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *****************************************************************************
*/

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
  if (sock != -1) {
    DrvWiFi->ES_WIFI_SetConnectionParam(sock, ES_WIFI_TCP_CONNECTION, _port);
    DrvWiFi->ES_WIFI_StartServerSingleConn(sock, COMM_SPI);
  }
}

WiFiClient WiFiServer::available(byte *status)
{
  static int cycle_server_down = 0;
  const int TH_SERVER_DOWN = 50;

  uint8_t sock = DrvWiFi->getCurrentSocket();
  WiFiClient client(sock);
  uint8_t _status = client.status();

  if (status != NULL) {
    *status = _status;
  }

  //server not in listen state, restart it
  if (cycle_server_down++ > TH_SERVER_DOWN) {
    DrvWiFi->ES_WIFI_SetConnectionParam(sock, ES_WIFI_TCP_CONNECTION, _port);
    DrvWiFi->ES_WIFI_StartServerSingleConn(sock, COMM_SPI);
    cycle_server_down = 0;
  }

  if (_status == SOCKET_BUSY) {
    return client;
  }
  return WiFiClient(255);
}

/**
 * @brief  Get server state
   @param  None
 * @retval None
 */
uint8_t WiFiServer::status()
{
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

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    if (DrvWiFi->getSocketState(sock) != SOCK_NOT_AVAIL) {
      WiFiClient client(sock);

      if (client.status() == SOCKET_BUSY) {
        n += client.write(buffer, size);
      }
    }
  }
  return n;
}

/******************* (C) COPYRIGHT 2017 Wi6labs ***********END OF FILE********/
