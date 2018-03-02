/**
  ******************************************************************************
  * @file    wifi_drv.h
  * @author  Wi6labs
  * @brief   driver abstraction layer.
  *****************************************************************************
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


/* Includes -----------------------------------------------------------------*/

#ifndef WiFi_Drv_H
#define WiFi_Drv_H

#include <stdint.h>
#include "IPAddress.h"
#include "es_wifi_conf.h"

/* Exported typedef ----------------------------------------------------------*/

typedef enum {
  SOCKET_FREE = 0,
  SOCKET_BUSY
} socket_status_t;

typedef enum {
  ES_WIFI_STATUS_OK             = 0,
  ES_WIFI_STATUS_REQ_DATA_STAGE = 1,
  ES_WIFI_STATUS_ERROR          = 2,
  ES_WIFI_STATUS_TIMEOUT        = 3,
  ES_WIFI_STATUS_IO_ERROR       = 4,
}ES_WIFI_Status_t;

typedef enum {
  ES_WIFI_MODE_SINGLE           = 0,
  ES_WIFI_MODE_MULTI            = 1,
}ES_WIFI_ConnMode_t;

typedef enum {
  ES_WIFI_TCP_CONNECTION        = 0,
  ES_WIFI_UDP_CONNECTION        = 1,
  ES_WIFI_UDP_LITE_CONNECTION   = 2,
  ES_WIFI_TCP_SSL_CONNECTION    = 3,
  ES_WIFI_MQTT_CONNECTION       = 4,
}ES_WIFI_ConnType_t;

/* Security settings for Wi-Fi network */
typedef enum {
  ES_WIFI_SEC_OPEN = 0x00,          /*!< Wi-Fi is open */
  ES_WIFI_SEC_WEP  = 0x01,          /*!< Wired Equivalent Privacy option for Wi-Fi security. \note This mode can't be used when setting up ES_WIFI Wi-Fi */
  ES_WIFI_SEC_WPA  = 0x02,          /*!< Wi-Fi Protected Access */
  ES_WIFI_SEC_WPA2 = 0x03,          /*!< Wi-Fi Protected Access 2 */
  ES_WIFI_SEC_WPA_WPA2= 0x04,       /*!< Wi-Fi Protected Access with both modes */
  ES_WIFI_SEC_WPA2_TKIP= 0x05,      /*!< Wi-Fi Protected Access with both modes */
  ES_WIFI_SEC_UNKNOWN = 0xFF,       /*!< Wi-Fi Unknown Security mode */
} ES_WIFI_SecurityType_t;

typedef enum {
  ES_WIFI_IPV4 = 0x00,
  ES_WIFI_IPV6 = 0x01,
} ES_WIFI_IPVer_t;

typedef enum {
  ES_WIFI_AP_NONE     = 0x00,
  ES_WIFI_AP_ASSIGNED = 0x01,
  ES_WIFI_AP_JOINED   = 0x02,
  ES_WIFI_AP_ERROR    = 0xFF,
} ES_WIFI_APState_t;

/* Communication interface */
typedef enum {
        COMM_UART,
        COMM_SPI,
        COMM_USB_HID,
        COMM_USB_VCP
} comm_mode;

/* virtual class necessary to abstract the Wi-Fi device                       */
class WiFiDrvClass {

protected :

public :

  virtual ES_WIFI_Status_t ES_WIFI_Init() = 0;
  virtual void ES_WIFI_ListAccessPoints() = 0;
  virtual int ES_WIFI_GetApNbr() = 0;
  virtual ES_WIFI_Status_t ES_WIFI_Connect(const char* SSID, const char* Password, ES_WIFI_SecurityType_t SecType) = 0;
  virtual void ES_WIFI_Disconnect() = 0;
  virtual uint8_t* ES_WIFI_GetMACAddress(uint8_t *mac) = 0;
  virtual IPAddress ES_WIFI_GetIPAddress() = 0;
  virtual IPAddress ES_WIFI_GetSubnetMask() = 0;
  virtual IPAddress ES_WIFI_GetGatewayIP() = 0;
  virtual uint8_t* ES_WIFI_GetSSID() = 0;
  virtual uint8_t* ES_WIFI_GetSSID(uint8_t networkItem) = 0;
  virtual uint8_t* ES_WIFI_GetBSSID(uint8_t *bbsid) = 0;
  virtual int ES_WIFI_GetEncryptionType() = 0;
  virtual int ES_WIFI_GetEncryptionType(uint8_t networkItem) = 0;
  virtual int32_t ES_WIFI_GetRSSI() = 0;
  virtual int32_t ES_WIFI_GetRSSI(uint8_t networkItem) = 0;
  virtual uint8_t* ES_WIFI_GetProductID() = 0;
  virtual char* ES_WIFI_GetFWRevID() = 0;
  virtual uint8_t* ES_WIFI_GetRTOSRev() = 0;
  virtual uint8_t* ES_WIFI_GetProductName() = 0;
  virtual uint8_t* ES_WIFI_GetAPIRev() = 0;
  virtual uint8_t* ES_WIFI_GetStackRev() = 0;
  virtual void ES_WIFI_SetMACAddress(uint8_t *mac) = 0;
  virtual void ES_WIFI_ResetToFactoryDefault() = 0;
  virtual void  ES_WIFI_ResetModule() = 0;
  virtual void ES_WIFI_SetProductName(uint8_t *ProductName) = 0;
  virtual void ES_WIFI_Ping(uint8_t *address, uint16_t count, uint16_t interval_ms) = 0;
  virtual void ES_WIFI_DNS_LookUp(const char *url, IPAddress *ipaddress) = 0;
  virtual void ES_WIFI_getRemoteData(uint8_t sock, uint8_t *ip, uint16_t *port) = 0;
  virtual void ES_WIFI_StartServerSingleConn(uint8_t index, comm_mode mode) = 0;
  virtual void ES_WIFI_StopServerSingleConn(uint8_t index) = 0;
  virtual void ES_WIFI_StartServerMultiConn(uint8_t socket, comm_mode mode);
  virtual void ES_WIFI_StopServerMultiConn();
  virtual void ES_WIFI_StartClientConnection(uint8_t index) = 0;
  virtual void ES_WIFI_StopClientConnection(uint8_t index) = 0;
  virtual void ES_WIFI_ReceiveData(uint8_t Socket, uint8_t *pdata, uint16_t Reqlen, uint16_t *Receivedlen, uint32_t Timeout)=0;
  virtual void ES_WIFI_SetConnectionParam(uint8_t Number, ES_WIFI_ConnType_t Type, uint16_t LocalPort) = 0;
  virtual void ES_WIFI_SetConnectionParam(uint8_t Number, ES_WIFI_ConnType_t Type, uint16_t LocalPort, IPAddress Ip) = 0;
  virtual void ES_WIFI_SendResp(uint8_t Socket, uint8_t *pdata, uint16_t Reqlen , uint16_t *SentLen , uint32_t Timeout) = 0;
  virtual uint8_t getCurrentSocket(void) = 0;
  virtual int8_t getFreeSocket(void) = 0;
  virtual uint8_t getSocketState(uint8_t socket) = 0;
};
#endif /*WiFi_Drv_H*/

/******************* (C) COPYRIGHT 2017 Wi6labs ***********END OF FILE********/
