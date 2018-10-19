/**
  ******************************************************************************
  * @file    WiFiST.h
  * @author  Wi6labs
  * @brief   WiFi API header.
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

#ifndef WiFiST_H
#define WiFiST_H

#include "wifi_drv.h"
#include "spi_drv.h"
#include "IPAddress.h"
#include "wl_definitions.h"
#include "WiFiClientST.h"

class WiFiClass {

private:
uint8_t wifi_status;
void init();

public:

  // WiFi device with SPI
  WiFiClass(SPIClass *SPIx, uint8_t cs, uint8_t spiIRQ, uint8_t reset, uint8_t wakeup);
  // WiFi device with UART
  WiFiClass(HardwareSerial *UARTx, uint8_t reset, uint8_t wakeup);
  // WiFi device with USB
  WiFiClass(uint8_t tx, uint8_t rx, uint8_t reset, uint8_t wakeup);

  int begin(char* ssid);
  int begin(char* ssid, uint8_t key_idx, const char* key);
  int begin(char* ssid, const char *passphrase, ES_WIFI_SecurityType_t sectype = ES_WIFI_SEC_WPA_WPA2);
  void disconnect(void);
  uint8_t status();
  char* firmwareVersion();
  IPAddress localIP();
  uint8_t* macAddress(uint8_t* mac);
  IPAddress subnetMask();
  IPAddress gatewayIP();
  char* SSID();
  char*	SSID(uint8_t networkItem);
  uint8_t* BSSID(uint8_t* bssid);
  int32_t RSSI();
  int32_t RSSI(uint8_t networkItem);
  uint8_t encryptionType();
  uint8_t	encryptionType(uint8_t networkItem);
  void setMac(uint8_t* macAddress);
  int8_t scanNetworks();
  static uint8_t getSocket();
  int hostByName(const char* aHostname, IPAddress aResult);
};

extern WiFiDrvClass *DrvWiFi;

#endif //WiFiST_H

/******************* (C) COPYRIGHT 2017 Wi6labs ***********END OF FILE********/
