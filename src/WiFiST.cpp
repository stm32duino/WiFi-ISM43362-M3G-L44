/**
  ******************************************************************************
  * @file    WiFiST.cpp
  * @author  Wi6labs
  * @brief   WiFi API.
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
#include "ISM43362_M3G_L44_driver.h"
#include "wl_definitions.h"

/**
* @brief  Initialize the WiFi only if shield is not yet detected
* @param  None
* @retval None
*/
void WiFiClass::init() {
  if (wifi_status == WL_NO_SHIELD)
  {
    if (DrvWiFi->ES_WIFI_Init() != ES_WIFI_STATUS_OK)
    {
      wifi_status = WL_NO_SHIELD;
    }
    else
    {
      wifi_status = WL_IDLE_STATUS;
    }
  }
}

/**
* @brief  Constructor.
* @param  SPIx   : SPI interface
* @param  cs     : chip select pin
* @param  spiIRQ : SPI IRQ
* @param  reset  : reset pin
* @param  wakeup : wakeup pin
* @retval None
*/
WiFiClass::WiFiClass(SPIClass *SPIx, uint8_t cs, uint8_t spiIRQ, uint8_t reset, uint8_t wakeup)
{
  DrvWiFi = new IsmDrvClass(SPIx, cs, spiIRQ, reset, wakeup);
  wifi_status = WL_NO_SHIELD;
}

/**
* @brief  Constructor.
* @param  UARTx   : UART interface
* @param  reset   : reset pin
* @param  wakeup  : wakeup pin
* @retval None
*/
WiFiClass::WiFiClass(HardwareSerial *UARTx, uint8_t reset, uint8_t wakeup)
{
  DrvWiFi = new IsmDrvClass(UARTx, reset, wakeup);
  wifi_status = WL_NO_SHIELD;
}

/**
* @brief  Constructor.
* @param  tx      : tx pin for USB
* @param  rx      : rx pin for USB
* @param  reset   : reset pin
* @param  wakeup  : wakeup pin
* @retval None
*/
WiFiClass::WiFiClass(uint8_t tx, uint8_t rx, uint8_t reset, uint8_t wakeup)
{
  DrvWiFi = new IsmDrvClass(tx, rx, reset, wakeup);
  wifi_status = WL_NO_SHIELD;
}

/**
* @brief  Initialize the WiFi if is not yet Initialize. Return device status.
* @param  None
* @retval Device status.
*/
uint8_t WiFiClass::status() {
  WiFiClass::init();
  return wifi_status;
  }

/**
* @brief  Start WiFi connection for OPEN networks
* @param  ssid: Pointer to the SSID string
* @retval WL_CONNECTED if success
*/
int WiFiClass::begin(char* ssid)
{
  char dummy_password[] = "DEADBEEFDEADFOODDEADBEEFED";   // Password not use because open connection
  ES_WIFI_SecurityType_t sectype = ES_WIFI_SEC_OPEN;

  WiFiClass::init();
  if (DrvWiFi->ES_WIFI_Connect(ssid, dummy_password, sectype) == ES_WIFI_STATUS_OK)
  {
    wifi_status = WL_CONNECTED;
    return WL_CONNECTED;
  }
  else
  {
    // Close connection
    DrvWiFi->ES_WIFI_Disconnect();
  }
  wifi_status = WL_CONNECT_FAILED;
  return WL_CONNECT_FAILED;
}

/**
* @brief  Start WiFi connection with WEP encryption.
*         Configure a key into the device. The key type (WEP-40, WEP-104) is
*         determined by the size of the key (5 bytes for WEP-40, 13 bytes for WEP-104).
* @param  ssid: Pointer to the SSID string.
* @param  key_idx: The key index to set. Valid values are 0-3.
* @param  key: Key input buffer.
* @retval WL_CONNECTED if success
*/
int WiFiClass::begin(char* ssid, uint8_t key_idx, const char* key)
{
  ES_WIFI_SecurityType_t sectype = ES_WIFI_SEC_WEP;

  UNUSED(key_idx); // Parameter unused, Arduino compatibility
  WiFiClass::init();

  if (DrvWiFi->ES_WIFI_Connect(ssid, key, sectype) == ES_WIFI_STATUS_OK)
  {
    wifi_status = WL_CONNECTED;
    return WL_CONNECTED;
  }
  else
  {
    // Close connection
    DrvWiFi->ES_WIFI_Disconnect();
  }
  wifi_status = WL_CONNECT_FAILED;
  return WL_CONNECT_FAILED;
}

/*
* @brief  Start WiFi connection with passphrase
* @param  ssid: Pointer to the SSID string.
* @param  passphrase: Passphrase. Valid characters in a passphrase
*         must be between ASCII 32-126 (decimal).
* @param  sectype: Security type (default: ES_WIFI_SEC_WPA_WPA2)
*/
int WiFiClass::begin(char* ssid, const char *passphrase, ES_WIFI_SecurityType_t sectype)
{
  WiFiClass::init();
  if (DrvWiFi->ES_WIFI_Connect(ssid, passphrase, sectype) == ES_WIFI_STATUS_OK)
  {
    wifi_status = WL_CONNECTED;
    return WL_CONNECTED;
  }
  else
  {
    // Close connection
    DrvWiFi->ES_WIFI_Disconnect();
  }
  wifi_status = WL_CONNECT_FAILED;
  return WL_CONNECT_FAILED;
}

/*
* @brief  Disconnect the WiFi shield from the current network
* @param  none
* @retval none
*/
void WiFiClass::disconnect(void)
{
  if (wifi_status != WL_NO_SHIELD) {
    DrvWiFi->ES_WIFI_Disconnect();
    wifi_status = WL_DISCONNECTED;
  }
}

/*
* @brief  Get firmware version
* @param  None
* @retval the firmware version
*/
char* WiFiClass::firmwareVersion() {
  return DrvWiFi->ES_WIFI_GetFWRevID();
}

/*
* @brief  Get the interface IP address
* @param  None
* @retval Ip address value
*/
 IPAddress WiFiClass::localIP() {
  return DrvWiFi->ES_WIFI_GetIPAddress();
 }

/*
* @brief  Get the interface MAC address
* @param  mac : pointer where to write MAC address
* @retval the MAC address
*/
uint8_t* WiFiClass::macAddress(uint8_t* mac)
{
  DrvWiFi->ES_WIFI_GetMACAddress(mac);
  return mac;
}

/*
* @brief  Get the interface subnet mask address
* @param  None
* @retval subnet mask address value
*/
 IPAddress WiFiClass::subnetMask() {
  return DrvWiFi->ES_WIFI_GetSubnetMask();
}

/*
* @brief  Get the gateway IP address
* @param  None
* @retval gateway IP address value
*/
 IPAddress WiFiClass::gatewayIP() {
  return DrvWiFi->ES_WIFI_GetGatewayIP();
}

/*
* @brief  Return the current SSID associated with the network
* @param  None
* @retval ssid string
*/
 char* WiFiClass::SSID() {
  return (char*)DrvWiFi->ES_WIFI_GetSSID();
}

/*
* @brief  Return the SSID discovered during the network scan.
* @param  networkItem: specify from which network item want to get the information
* @retval ssid string of the specified item on the networks scanned list
*/
char* WiFiClass::SSID(uint8_t networkItem) {

  return (char*)DrvWiFi->ES_WIFI_GetSSID(networkItem);
}

/*
* @brief  Return the current BSSID associated with the network.
*         It is the MAC address of the Access Point
* @param  pointer where to write the bbsid
* @retval the bbsid
*/
uint8_t* WiFiClass::BSSID(uint8_t* bssid)
{
  DrvWiFi->ES_WIFI_GetBSSID(bssid);
  return bssid;
}

/*
* @brief  Return the current RSSI /Received Signal Strength (in dBm)
*         associated with the network
* @param  None
* @retval RSSI
*/
int32_t WiFiClass::RSSI()
{
  return DrvWiFi->ES_WIFI_GetRSSI();
}

/*
* @brief  Return the RSSI of the networks discovered during the scanNetworks
* @param  networkItem: specify from which network item want to get the information
* @retval RSSI of the specified item on the networks scanned list
*/
int32_t WiFiClass::RSSI(uint8_t networkItem)
{
  return DrvWiFi->ES_WIFI_GetRSSI(networkItem);
}

/*
* @brief  Return the Encryption Type associated with the network
* @param  None
* @retval one value of ES_WIFI_SecurityType_t enum
*/
uint8_t WiFiClass::encryptionType()
{
  return DrvWiFi->ES_WIFI_GetEncryptionType();
}

/*
* @brief  Return the encryption type of the networks discovered during the scanNetworks
* @param  networkItem: specify from which network item want to get the information
* @retval encryption type (enum ES_WIFI_SecurityType_t) of the specified item on the networks scanned list
*/
uint8_t WiFiClass::encryptionType(uint8_t networkItem)
{
  return DrvWiFi->ES_WIFI_GetEncryptionType(networkItem);
}

/*
* @brief  Set the MAC addresse of the WiFi device on the board.
* @param  mac : mac Address to set
* @retval None
*/
void WiFiClass::setMac(uint8_t* macAddress)
{
  DrvWiFi->ES_WIFI_SetMACAddress(macAddress);
}

/*
* @brief  Start scan WiFi networks available
* @param  None
* @retval Number of discovered networks
*/
int8_t WiFiClass::scanNetworks()
{
  DrvWiFi->ES_WIFI_ListAccessPoints();
  if (DrvWiFi->ES_WIFI_GetApNbr() == 0)
  {
    wifi_status = WL_NO_SSID_AVAIL;
  }
  else
  {
    wifi_status = WL_SCAN_COMPLETED;
  }
  return DrvWiFi->ES_WIFI_GetApNbr();
}

/*
* @brief  Get the first socket available
* @param  None
* @retval None
*/
uint8_t WiFiClass::getSocket()
{
  return DrvWiFi->getFreeSocket();
}

/*
* @brief  Resolve the given hostname to an IP address.
* @param  aHostname: Name to be resolved
* @param  aResult: IPAddress structure to store the returned IP address
* @retval 1 if aIPAddrString was successfully converted to an IP address
*/
int WiFiClass::hostByName(const char* aHostname, IPAddress aResult)
{
  DrvWiFi->ES_WIFI_DNS_LookUp(aHostname, &aResult);
  return 1;
}

WiFiDrvClass *DrvWiFi;

/******************* (C) COPYRIGHT 2017 Wi6labs ***********END OF FILE********/
