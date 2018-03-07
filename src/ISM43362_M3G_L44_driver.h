/**
  ******************************************************************************
  * @file    ISM43362_M3G_L44_driver.h
  * @author  MCD Application Team, Wi6labs
  * @brief   header file for the es-wifi module.
  * original file in STM32Cube_FW_L4_V1.10.0, file es_wifi.h
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ES_WIFI_H
#define __ES_WIFI_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "spi_drv.h"
#include "wifi_drv.h"
#include "wl_definitions.h"

/* Exported macro-------------------------------------------------------------*/
#define MIN(a, b)  ((a) < (b) ? (a) : (b))

#define AT_OK_STRING "\r\nOK\r\n> "
#define AT_OK_STRING_LEN (sizeof(AT_OK_STRING) - 1)
#define AT_ERROR_STRING "\r\nERROR"

/* List of AT Command                                                        */
/* -------------------------access point command---------------------------- */
#define AT_AP_ACTIVATE                   "A0"
#define AT_AP_SET_SECURITY_MODE          "A1"
#define AT_AP_SET_SECURITY_KEY           "A2"
#define AT_AP_GET_DHCP_CACHE_ADDR        "AA"
#define AT_AP_SET_CHANNEL                "AC"
#define AT_AP_ACTIVATE_DIRECT_MODE       "AD"
#define AT_AP_EXIT_DIRECT_MODE           "AE"
#define AT_AP_SET_LEASE_TIME             "AL"
#define AT_AP_GET_RSSI                   "AR"
#define AT_AP_SET_SSID                   "AS"
#define AT_AP_SET_MAX_CLIENT             "AT"
#define AT_AP_SHOW_SETTING               "A?"

/* ---------------------------spi command----------------------------------- */
#define AT_SPI_SET_MODE                  "B2"
#define AT_SPI_SET_READ_PIN              "B3"

#define AT_SHOW_COMM_INTERFACE           "B?"

/* ---------------------------network command-------------------------------- */
#define AT_NET_JOIN                      "C0"
#define AT_NET_SET_SSID                  "C1"
#define AT_NET_SET_PASSPHRASE            "C2"
#define AT_NET_SET_SECURITY_TYPE         "C3"
#define AT_NET_SET_DHCP                  "C4"
#define AT_NET_SET_IP_VER                "C5"
#define AT_NET_SET_IP_ADDR               "C6"
#define AT_NET_SET_IP_MASK               "C7"
#define AT_NET_SET_GATEWAY               "C8"
#define AT_NET_SET_FIRST_DNS             "C9"
#define AT_NET_SET_SECOND_DNS            "CA"
#define AT_NET_SET_JOIN_RETRY_CNT        "CB"
#define AT_NET_AUTO_CONNECT              "CC"
#define AT_NET_DISCONNECT                "CD"
#define AT_NET_SET_AUTH_TYPE             "CE"
#define AT_NET_PACKET_FILTER             "CF"
#define AT_NET_IGMP_GROUP                "CJ"
#define AT_NET_MAC_ALLOW_LIST            "CM"
#define AT_NET_SET_COUNTRY_CODE          "CN"
#define AT_NET_GET_RSSI                  "CR"
#define AT_NET_GET_CONNECTION_STATUS     "CS"
#define AT_NET_CONNECT_WPS               "CW"
#define AT_NET_SHOW_SETTINGS             "C?"

/* ---------------------------dns command----------------------------------- */
#define AT_DNS_LOOKUP                    "D0"
#define AT_ENABLE_mDNS                   "D1"
#define AT_ENABLE_mDNS_SERVICE           "D2"

#define AT_KEEP_ALIVE                    "E2"

/* ---------------------------scan command---------------------------------- */
#define AT_SCAN                          "F0"
#define AT_SCAN_REPEAT_CNT               "F1"
#define AT_SCAN_SET_DELAY_MS             "F2"
#define AT_SCAN_SET_CHANNEL              "F3"
#define AT_SCAN_SET_BBSID                "F4"
#define AT_SCAN_SET_SSID                 "F5"
#define AT_SCAN_SHOW_SETTINGS            "F?"

/* ---------------------------GPIO command---------------------------------- */
#define AT_GPIO_READ_MODE                "G2"
#define AT_GPIO_WRITE                    "G3"
#define AT_GPIO_SETUP                    "G4"
#define AT_GPIO_SHOW_SETTINGS            "G?"

#define AT_API_SHOW_SETTINGS             "I?"

/* ---------------------------message command------------------------------- */
#define AT_MESSAGE_TEST_SERIAL_FLASH     "MF"
#define AT_MESSAGE_READ                  "MR"
#define AT_MESSAGE_DELETE                "MS"
#define AT_MESSAGE_SET                   "MT"

/* --------------------------transport command------------------------------ */
#define AT_TR_SET_SOCKET                 "P0"
#define AT_TR_SET_PROTOCOL               "P1"
#define AT_TR_SET_LOCAL_PORT_NUMBER      "P2"
#define AT_TR_SET_REMOTE_HOST_IP_ADDR    "P3"
#define AT_TR_SET_REMOTE_PORT_NUMBER     "P4"
#define AT_TR_SERVER                     "P5"
#define AT_TR_CLIENT                     "P6"
#define AT_TR_REQUEST_TCP_LOOP           "P7"
#define AT_TR_SET_LISTEN_BACKLOGS        "P8"
#define AT_TR_SET_CERTIFICATION_VERIF    "P9"
#define AT_TR_SET_CCA                    "PA"
#define AT_TR_SET_CCA_RESULTS            "PB"
#define AT_TR_SECURITY_CERTIFICATES      "PC"
#define AT_TR_SECURITY_KEYS              "PD"
#define AT_TR_TCP_KEEP_ALIVE             "PK"
#define AT_TR_PBM_READ_MODE              "PR"
#define AT_TR_ETHERNET_PACKET_STAT       "PS"
#define AT_TR_SEQUENCING                 "PT"
#define AT_TR_PBM_WRITE_PACKET           "PW"
#define AT_TR_SET_TCP_MESSAGE_TIMEOUT    "PY"
#define AT_TR_SHOW_SETTNGS               "P?"

/* --------------------------read command----------------------------------- */
#define AT_READ_DATA                     "R0=\r"
#define AT_READ_SET_PACKET_SIZE          "R1"
#define AT_READ_SET_TIMEOUT_MS           "R2"
#define AT_READ_RECEIVE_MODE             "R3"
#define AT_READ_SHOW_SETTINGS            "R?"

/* --------------------------write command---------------------------------- */
#define AT_WRITE_DATA                    "S0"
#define AT_WRITE_SET_PACKET_SIZE         "S1"
#define AT_WRITE_SET_TIMEOUT             "S2"
#define AT_WRITE_DATA_WITH_LENGTH        "S3"
#define AT_WRITE_SHOW_SETTINGS           "S?"

/* --------------------------ping command----------------------------------- */
#define AT_PING_TARGET_ADDR              "T0"
#define AT_PING_SET_TARGET_ADDR          "T1"
#define AT_PING_SET_REPEAT_COUNT         "T2"
#define AT_PING_SET_DELAY_MS             "T3"
#define AT_PING_SHOW_SETTINGS            "T?"

/* --------------------------uart command----------------------------------- */
#define AT_UART_ENABLE                   "U0"
#define AT_UART_SET_BAUDRATE             "U2"
#define AT_UART_SHOW_SETTINGS            "U?"

#define AT_WLAN_GPIO                     "WL"

/* --------------------------system command--------------------------------- */
#define AT_SYS_SET_RESET_FACTORY         "Z0"
#define AT_SYS_SAVE_SETTINGS             "Z1"
#define AT_SYS_CLEAR_SETTINGS            "Z2"
#define AT_SYS_SET_FACTORY               "Z3"
#define AT_SYS_SET_MAC_ADDR              "Z4"
#define AT_SYS_GET_MAC_ADDR              "Z5"
#define AT_SEYS_SET_AP_IP_ADDR           "Z6"
#define AT_SYS_SET_WPS_PIN               "Z7"
#define AT_SYS_GET_WPS_PIN               "Z8"
#define AT_SYS_SET_USB                   "Z9"
#define AT_SYS_CLEAR_FACTOR_LOCK         "ZC"
#define AT_SYS_FLASH_DUMP                "ZD"
#define AT_SYS_SET_FACTORY_LOCK          "ZF"
#define AT_SYS_SET_PRODUCT_NAME          "ZN"
#define AT_SYS_OTA_GET_URL               "ZO"
#define AT_SYS_PWR_MNGT                  "ZP"
#define AT_SYS_RESET_MODULE              "ZR"
#define AT_SYS_FIRMWARE_UPGRADE          "ZU"
#define AT_SYS_SET_OTA_METHOD            "ZV"
#define AT_SYS_SHOW_SETTINGS             "Z?"

#define SUFFIX_CMD                       "\r"

typedef struct
{
  uint32_t Port;
  uint32_t BaudRate;
  uint32_t DataWidth;
  uint32_t Parity;
  uint32_t StopBits;
  uint32_t Mode;
}ES_WIFI_UARTConfig_t;


typedef struct
{
  uint32_t Configuration;
  uint32_t WPSPin;
  uint32_t VID;
  uint32_t PID;
  uint8_t MAC[6];
  uint8_t AP_IPAddress[4];
  uint32_t PS_Mode;
  uint32_t RadioMode;
  uint32_t CurrentBeacon;
  uint32_t PrevBeacon;
  uint32_t ProductName;
}ES_WIFI_SystemConfig_t;

typedef struct {
  uint8_t* Address;                        /*!< Pointer to domain or IP to ping */
  uint32_t Time;                           /*!< Time in milliseconds needed for pinging */
  uint8_t Success;                         /*!< Status indicates if ping was successful */
}ES_WIFI_Ping_t;

typedef struct {
  uint8_t SSID[ES_WIFI_MAX_SSID_NAME_SIZE + 1]; /*!< Service Set Identifier value.Wi-Fi spot name */
  ES_WIFI_SecurityType_t Security;         /*!< Security of Wi-Fi spot.  */
  int16_t RSSI;                            /*!< Signal strength of Wi-Fi spot */
  uint8_t MAC[6];                          /*!< MAC address of spot */
  uint8_t Channel;                         /*!< Wi-Fi channel */
} ES_WIFI_AP_t;

typedef struct {
  uint8_t SSID[ES_WIFI_MAX_SSID_NAME_SIZE + 1];  /*!< Network public name for ESP AP mode */
  uint8_t IP_Addr[4];                       /*!< IP Address */
  uint8_t MAC_Addr[6];                      /*!< MAC address */
} ES_WIFI_APSettings_t;

typedef struct {
  ES_WIFI_AP_t AP[ES_WIFI_MAX_DETECTED_AP];
  uint8_t nbr;
}ES_WIFI_APs_t;

typedef struct {
  uint8_t          SSID[ES_WIFI_MAX_SSID_NAME_SIZE + 1];
  uint8_t          pswd[ES_WIFI_MAX_PSWD_NAME_SIZE + 1];
  ES_WIFI_SecurityType_t Security;
  uint8_t          DHCP_IsEnabled;
  uint8_t          JoinRetries;
  uint8_t          IsConnected;
  uint8_t          AutoConnect;
  ES_WIFI_IPVer_t  IP_Ver;
  uint8_t          IP_Addr[4];
  uint8_t          IP_Mask[4];
  uint8_t          Gateway_Addr[4];
  uint8_t          DNS1[4];
  uint8_t          DNS2[4];
} ES_WIFI_Network_t;

typedef struct {
  ES_WIFI_ConnType_t Type;
  uint8_t            Number;
  uint16_t           RemotePort;
  uint16_t           LocalPort;
  uint8_t            RemoteIP[4];
  char*              Name;
} ES_WIFI_Conn_t;

typedef struct {
  uint8_t           Product_ID[ES_WIFI_PRODUCT_ID_SIZE];
  uint8_t           FW_Rev[ES_WIFI_FW_REV_SIZE];
  uint8_t           API_Rev[ES_WIFI_API_REV_SIZE];
  uint8_t           Stack_Rev[ES_WIFI_STACK_REV_SIZE];
  uint8_t           RTOS_Rev[ES_WIFI_RTOS_REV_SIZE];
  uint8_t           Product_Name[ES_WIFI_PRODUCT_NAME_SIZE];
  uint32_t          CPU_Clock;
  ES_WIFI_SecurityType_t Security;
  ES_WIFI_Network_t NetSettings;
  ES_WIFI_APSettings_t APSettings;
  uint8_t            CmdData[ES_WIFI_DATA_SIZE];
  uint32_t           Timeout;
  uint32_t           BufferSize;
}ES_WIFIObject_t;

class IsmDrvClass : public WiFiDrvClass {

private :
  DriverClass *Drv;                             // pointer on means of communication
  ES_WIFIObject_t EsWifiObj;                    // settings pointer
  ES_WIFI_SystemConfig_t ESWifiSysObj;          // configuration pointer
  ES_WIFI_APs_t ESWifiApObj;                    // list of network
  ES_WIFI_Conn_t ESWifiConnTab[MAX_SOCK_NUM];   // local table to know which socket are use
  uint8_t currentSock;                          // the current socket
  socket_status_t sockState[MAX_SOCK_NUM];      // socket status

  uint8_t Hex2Num(char a);
  uint32_t ParseHexNumber(char* ptr, uint8_t* cnt);
  int32_t ParseNumber(char* ptr, uint8_t* cnt);
  void ParseMAC(char* ptr, uint8_t* arr);
  void ParseIP(char* ptr, uint8_t* arr);
  ES_WIFI_SecurityType_t ParseSecurity(char* ptr);
  void AT_ParseInfo(uint8_t *pdata);
  void AT_ParseAP(char *pdata, ES_WIFI_APs_t *APs);
  void AT_ParseSystemConfig(char *pdata, ES_WIFI_SystemConfig_t *pConfig);
  void AT_ParseConnSettings(char *pdata, ES_WIFI_Network_t *NetSettings);
  void AT_ParseTrSettings(char *pdata, ES_WIFI_Conn_t *ConnSettings);
  ES_WIFI_Status_t AT_ExecuteCommand(uint8_t* cmd, uint8_t *pdata);
  ES_WIFI_Status_t AT_RequestSendData(uint8_t* cmd, uint16_t len, uint8_t *pdata);
  ES_WIFI_Status_t ReceiveShortDataLen(char *pdata, uint16_t Reqlen, uint16_t *ReadData);
  ES_WIFI_Status_t ReceiveLongDataLen(char *pdata, uint16_t Reqlen, uint16_t *ReadData);
  ES_WIFI_Status_t AT_RequestReceiveData(uint8_t* cmd, char *pdata, uint16_t Reqlen, uint16_t *ReadData);
  ES_WIFI_Status_t ES_WIFI_SetTimeout(uint32_t Timeout);
  ES_WIFI_Status_t ES_WIFI_GetNetworkSettings();
  ES_WIFI_Status_t ES_WIFI_GetSystemConfig();
  ES_WIFI_Status_t ES_WIFI_GetTrSettings(uint8_t index);

public:

  IsmDrvClass(SPIClass *SPIx, uint8_t cs, uint8_t spiIRQ, uint8_t reset, uint8_t wakeup);
  IsmDrvClass(HardwareSerial *UARTx, uint8_t reset, uint8_t wakeup);
  IsmDrvClass(uint8_t tx, uint8_t rx, uint8_t reset, uint8_t wakeup);

  virtual ES_WIFI_Status_t ES_WIFI_Init();
  virtual void ES_WIFI_ListAccessPoints();
  virtual int ES_WIFI_GetApNbr();
  virtual ES_WIFI_Status_t ES_WIFI_Connect(const char* SSID, const char* Password, ES_WIFI_SecurityType_t SecType);
  virtual void ES_WIFI_Disconnect();
  virtual uint8_t* ES_WIFI_GetMACAddress(uint8_t *mac);
  virtual IPAddress ES_WIFI_GetIPAddress();
  virtual IPAddress ES_WIFI_GetSubnetMask();
  virtual IPAddress ES_WIFI_GetGatewayIP();
  virtual uint8_t* ES_WIFI_GetSSID();
  virtual uint8_t* ES_WIFI_GetSSID(uint8_t networkItem);
  virtual uint8_t* ES_WIFI_GetBSSID(uint8_t *bssid);
  virtual int ES_WIFI_GetEncryptionType();
  virtual int ES_WIFI_GetEncryptionType(uint8_t networkItem);
  virtual int32_t ES_WIFI_GetRSSI();
  virtual int32_t ES_WIFI_GetRSSI(uint8_t networkItem);
  virtual uint8_t* ES_WIFI_GetProductID();
  virtual char* ES_WIFI_GetFWRevID();
  virtual uint8_t* ES_WIFI_GetRTOSRev();
  virtual uint8_t* ES_WIFI_GetProductName();
  virtual uint8_t* ES_WIFI_GetAPIRev();
  virtual uint8_t* ES_WIFI_GetStackRev();
  virtual void ES_WIFI_SetMACAddress(uint8_t *mac);
  virtual void ES_WIFI_ResetToFactoryDefault();
  virtual void ES_WIFI_ResetModule();
  virtual void ES_WIFI_SetProductName(uint8_t *ProductName);
  virtual void ES_WIFI_Ping(uint8_t *address, uint16_t count, uint16_t interval_ms);
  virtual void ES_WIFI_DNS_LookUp(const char *url, IPAddress *ipaddress);
  virtual void ES_WIFI_StartClientConnection(uint8_t index);
  virtual void ES_WIFI_StopClientConnection(uint8_t index);
  virtual void ES_WIFI_StartServerSingleConn(uint8_t index, comm_mode mode);
  virtual void ES_WIFI_StopServerSingleConn(uint8_t index);
  virtual void ES_WIFI_StartServerMultiConn(uint8_t socket, comm_mode mode);
  virtual void ES_WIFI_StopServerMultiConn();
  virtual void ES_WIFI_ReceiveData(uint8_t Socket, uint8_t *pdata, uint16_t Reqlen, uint16_t *Receivedlen, uint32_t Timeout);
  virtual void ES_WIFI_getRemoteData(uint8_t sock, uint8_t *ip, uint16_t *port);
  virtual void ES_WIFI_SetConnectionParam(uint8_t Number, ES_WIFI_ConnType_t Type, uint16_t LocalPort);
  virtual void ES_WIFI_SetConnectionParam(uint8_t Number, ES_WIFI_ConnType_t Type, uint16_t LocalPort, IPAddress Ip);
  virtual void ES_WIFI_SendResp(uint8_t Socket, uint8_t *pdata, uint16_t Reqlen , uint16_t *SentLen , uint32_t Timeout);
  virtual uint8_t getCurrentSocket(void);
  virtual int8_t getFreeSocket(void);
  virtual uint8_t getSocketState(uint8_t socket);
};

#endif /*__ES_WIFI_H*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/ 
