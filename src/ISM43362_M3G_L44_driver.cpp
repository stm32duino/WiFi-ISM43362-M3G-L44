/**
  ******************************************************************************
  * @file    ISM43362_M3G_L44_driver.cpp
  * @author  MCD Application Team, Wi6labs
  * @brief   This file provides a set of functions to manage the es-wifi module.
  * original file in STM32Cube_FW_L4_V1.10.0, file es_wifi.c
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V. 
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

/* Includes ------------------------------------------------------------------*/
#include "ISM43362_M3G_L44_driver.h"


#define CHARISHEXNUM(x)                 (((x) >= '0' && (x) <= '9') || \
                                         ((x) >= 'a' && (x) <= 'f') || \
                                         ((x) >= 'A' && (x) <= 'F'))

#define CHARISNUM(x)                    ((x) >= '0' && (x) <= '9')
#define CHAR2NUM(x)                     ((x) - '0')

//#define DEBUG
#ifdef DEBUG
#define PRINTCMD(str) printf("%s\r\n", str)
#define PRINTDATA(str, len)   do {                                  \
                                for(uint16_t i = 0; i < len; i++)   \
                                  printf("%c", *(str+i));          \
                                printf("\n\r");                    \
                              }while(0);
#else
#define PRINTCMD(str)
#define PRINTDATA(str, len)
#endif //DEBUG

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Constructor.
 * @param  SPIx   : SPI interface
 * @param  cs     : chip select pin
 * @param  spiIRQ : SPI IRQ
 * @param  reset : reset pin
 * @param  wakeup : wakeup pin
 * @retval None
 */
IsmDrvClass::IsmDrvClass(SPIClass *SPIx, uint8_t cs, uint8_t spiIRQ,
uint8_t reset, uint8_t wakeup) {
  /* Call Spi constructor                                                    */
  Drv = new SpiDrvClass(SPIx, cs, spiIRQ, reset, wakeup);
  for (int i=0; i<MAX_SOCK_NUM; i++)
  {
    sockState[i] = SOCKET_FREE;
  }
  currentSock = 0;
}

/**
* @brief  Constructor.
* @param  UARTx   : UART interface
* @param  reset   : reset pin
* @param  wakeup  : wakeup pin
* @retval None
*/
IsmDrvClass::IsmDrvClass(HardwareSerial *UARTx,  uint8_t reset, uint8_t wakeup) {
  /* Call UART constructor                                                   */
  /* TODO: neither developped nor tested on DISCO L475VG IOT because the WiFi
    module is only connected by SPI. An UART driver needs to be added.
    This constructor is given as example. Maybe it needs to be adapted.
  */
  UNUSED(UARTx);
  UNUSED(reset);
  UNUSED(wakeup);

  // Drv = new UARTDrvClass(UARTx, reset, wakeup);
  for (int i=0; i<MAX_SOCK_NUM; i++)
  {
    sockState[i] = SOCKET_FREE;
  }
  currentSock = 0;
}

/**
* @brief  Constructor.
* @param  tx      : tx pin for USB
* @param  rx      : rx pin for USB
* @param  reset   : reset pin
* @param  wakeup  : wakeup pin
* @retval None
*/
IsmDrvClass::IsmDrvClass(uint8_t tx, uint8_t rx, uint8_t reset, uint8_t wakeup) {
  /* Call USB constructor                                                    */
  /* TODO: neither developped nor tested on DISCO L475VG IOT because the WiFi
    module is only connected by SPI. An USB driver needs to be added.
    This constructor is given as example. Maybe it needs to be adapted.
  */
  UNUSED(tx);
  UNUSED(rx);
  UNUSED(reset);
  UNUSED(wakeup);

  // Drv = new USBDrvClass(tx, rx, reset, wakeup);
  for (int i=0; i<MAX_SOCK_NUM; i++)
  {
    sockState[i] = SOCKET_FREE;
  }
  currentSock = 0;
}


/**
  * @brief  Convert char in Hex format to integer.
  * @param  a: character to convert
  * @retval integer value.
  */
uint8_t IsmDrvClass::Hex2Num(char a)
{
    if (a >= '0' && a <= '9') {            /* Char is num */
        return a - '0';
    } else if (a >= 'a' && a <= 'f') {     /* Char is lowercase character A - Z (hex) */
        return (a - 'a') + 10;
    } else if (a >= 'A' && a <= 'F') {     /* Char is uppercase character A - Z (hex) */
        return (a - 'A') + 10;
    }

    return 0;
}

/**
  * @brief  Extract a hex number from a string.
  * @param  ptr: pointer to string
  * @param  cnt: pointer to the number of parsed digit
  * @retval Hex value.
  */
uint32_t IsmDrvClass::ParseHexNumber(char* ptr, uint8_t* cnt)
{
    uint32_t sum = 0;
    uint8_t i = 0;

    if(ptr == NULL) {
      return 0;
    }

    while (CHARISHEXNUM(*ptr)) {    /* Parse number */
        sum <<= 4;
        sum += Hex2Num(*ptr);
        ptr++;
        i++;
    }

    if (cnt != NULL) {               /* Save number of characters used for number */
        *cnt = i;
    }
    return sum;                      /* Return number */
}

/**
  * @brief  Parses and returns number from string.
  * @param  ptr: pointer to string
  * @param  cnt: pointer to the number of parsed digit
  * @retval integer value.
  */
int32_t IsmDrvClass::ParseNumber(char* ptr, uint8_t* cnt)
{
    uint8_t minus = 0, i = 0;
    int32_t sum = 0;

    if(ptr == NULL) {
      return 0;
    }

    if (*ptr == '-') {                    /* Check for minus character */
        minus = 1;
        ptr++;
        i++;
    }
    while (CHARISNUM(*ptr)) {             /* Parse number */
        sum = 10 * sum + CHAR2NUM(*ptr);
        ptr++;
        i++;
    }
    if (cnt != NULL) {                    /* Save number of characters used for number */
        *cnt = i;
    }
    if (minus) {                          /* Minus detected */
        return 0 - sum;
    }
    return sum;                           /* Return number */
}

/**
  * @brief  Parses and returns MAC address.
  * @param  ptr: pointer to string
  * @param  arr: pointer to MAC array
  * @retval None.
  */
void IsmDrvClass::ParseMAC(char* ptr, uint8_t* arr)
{
  uint8_t hexnum = 0, hexcnt;

  if((ptr == NULL) || (arr == NULL)) {
    return;
  }

  while(* ptr) {
    hexcnt = 1;
    if(*ptr != ':')
    {
      arr[hexnum++] = ParseHexNumber(ptr, &hexcnt);
    }
    ptr = ptr + hexcnt;
  }
}

/**
  * @brief  Parses and returns IP address.
  * @param  ptr: pointer to string
  * @param  arr: pointer to IP array
  * @retval None.
  */
void IsmDrvClass::ParseIP(char* ptr, uint8_t* arr)
{
  uint8_t hexnum = 0, hexcnt;

  if((ptr == NULL) || (arr == NULL)) {
    return;
  }

  while(* ptr) {
    hexcnt = 1;
    if(*ptr != '.')
    {
      arr[hexnum++] = ParseNumber(ptr, &hexcnt);
    }
    ptr = ptr + hexcnt;
  }
}

/**
  * @brief  Parses Security type.
  * @param  ptr: pointer to string
  * @retval Encryption type.
  */
ES_WIFI_SecurityType_t IsmDrvClass::ParseSecurity(char* ptr)
{
  if(ptr == NULL) {
    return ES_WIFI_SEC_UNKNOWN;
  }

  if(strstr(ptr,"Open")) return ES_WIFI_SEC_OPEN;
  else if(strstr(ptr,"WEP")) return ES_WIFI_SEC_WEP;
  else if(strstr(ptr,"WPA WPA2")) return ES_WIFI_SEC_WPA_WPA2;
  else if(strstr(ptr,"WPA2 TKIP")) return ES_WIFI_SEC_WPA2_TKIP;
  else if(strstr(ptr,"WPA2")) return ES_WIFI_SEC_WPA2;
  else if(strstr(ptr,"WPA")) return ES_WIFI_SEC_WPA;
  else return ES_WIFI_SEC_UNKNOWN;
}

/**
  * @brief  Parses ES module informations and save them in the handle.
  * @param  ptr: pointer to string
  * @retval None.
  */
void IsmDrvClass::AT_ParseInfo(uint8_t *pdata)
{
  char *ptr;
  uint8_t num = 0;

  if(pdata == NULL) {
    return;
  }

  ptr = strtok((char *)pdata + 2, ",");

  while (ptr != NULL){
    switch (num++) {
    case 0:
      strncpy((char *)EsWifiObj.Product_ID,  ptr, ES_WIFI_PRODUCT_ID_SIZE);
      break;

    case 1:
      strncpy((char *)EsWifiObj.FW_Rev,  ptr, ES_WIFI_FW_REV_SIZE );
      break;

    case 2:
      strncpy((char *)EsWifiObj.API_Rev,  ptr, ES_WIFI_API_REV_SIZE);
      break;

    case 3:
      strncpy((char *)EsWifiObj.Stack_Rev,  ptr, ES_WIFI_STACK_REV_SIZE);
      break;

    case 4:
      strncpy((char *)EsWifiObj.RTOS_Rev,  ptr, ES_WIFI_RTOS_REV_SIZE);
      break;

    case 5:
      EsWifiObj.CPU_Clock = ParseNumber(ptr, NULL);
      break;

    case 6:
      ptr = strtok(ptr, "\r");
      strncpy((char *)EsWifiObj.Product_Name,  ptr, ES_WIFI_PRODUCT_NAME_SIZE);
      break;

    default: break;
    }
    ptr = strtok(NULL, ",");
  }
}

/**
  * @brief  Parses Access point configuration.
  * @param  APs: Access points structure
  * @param  ptr: pointer to string
  * @retval None.
  */
void IsmDrvClass::AT_ParseAP(char *pdata, ES_WIFI_APs_t *APs)
{
  uint8_t num = 0;
  char *ptr;

  if((pdata == NULL) || (APs == NULL)) {
    return;
  }

  APs->nbr = 0;
  ptr = strtok(pdata + 2, ",");

  while ((ptr != NULL) && (APs->nbr < ES_WIFI_MAX_DETECTED_AP)) {
    switch (num++) {
    case 0: /* Ignore index */
    case 4: /* Ignore Max Rate */
    case 5: /* Ignore Network Type */
    case 7: /* Ignore Radio Band */
      break;

    case 1:
      ptr[strlen(ptr) - 1] = 0;
      strncpy((char *)APs->AP[APs->nbr].SSID,  ptr+ 1, ES_WIFI_MAX_SSID_NAME_SIZE + 1);
      break;

    case 2:
      ParseMAC(ptr, APs->AP[APs->nbr].MAC);
      break;

    case 3:
      APs->AP[APs->nbr].RSSI = ParseNumber(ptr, NULL);
      break;

    case 6:
      APs->AP[APs->nbr].Security = ParseSecurity(ptr);
      break;

    case 8:
      APs->AP[APs->nbr].Channel = ParseNumber(ptr, NULL);
      APs->nbr++;
      num = 1;
      break;

    default:
      break;
    }
    ptr = strtok(NULL, ",");
  }
}

/**
  * @brief  Parses System configuration.
  * @param  APs: System Config structure
  * @param  ptr: pointer to string
  * @retval None.
  */
void IsmDrvClass::AT_ParseSystemConfig(char *pdata, ES_WIFI_SystemConfig_t *pConfig)
{
  uint8_t num = 0;
  char *ptr;

  if((pdata == NULL) || (pConfig == NULL)) {
    return;
  }

  ptr = strtok(pdata + 2, ",");

  while (ptr != NULL) {
    switch (num++) {
    case 0:
      pConfig->Configuration = ParseNumber(ptr, NULL);
      break;

    case 1:
      pConfig->WPSPin = ParseNumber(ptr, NULL);
      break;

    case 2:
      pConfig->VID = ParseNumber(ptr, NULL);
      break;

    case 3:
      pConfig->PID = ParseNumber(ptr, NULL);
      break;

    case 4:
      ParseMAC(ptr, pConfig->MAC);
      break;

    case 5:
      ParseIP(ptr, pConfig->AP_IPAddress);
      break;

    case 6:
      pConfig->PS_Mode = ParseNumber(ptr, NULL);
      break;

    case 7:
      pConfig->RadioMode = ParseNumber(ptr, NULL);
      break;

    case 8:
      pConfig->CurrentBeacon = ParseNumber(ptr, NULL);
      break;

    case 9:
      pConfig->PrevBeacon = ParseNumber(ptr, NULL);
      break;

    case 10:
      pConfig->ProductName = ParseNumber(ptr, NULL);
      break;

    default:
      break;
    }
    ptr = strtok(NULL, ",");
  }
}


/**
  * @brief  Parses WIFI connection settings.
  * @param  NetSettings: settings
  * @param  pdata: pointer to data
  * @retval None.
  */
void IsmDrvClass::AT_ParseConnSettings(char *pdata, ES_WIFI_Network_t *NetSettings)
{
  uint8_t num = 0;
  char *ptr;

  if((pdata == NULL) || (NetSettings == NULL)) {
    return;
  }

  ptr = strtok(pdata + 2, ",");

  while (ptr != NULL) {
    switch (num++) {
    case 0:
      strncpy((char *)NetSettings->SSID,  ptr, ES_WIFI_MAX_SSID_NAME_SIZE + 1);
      break;

    case 1:
      strncpy((char *)NetSettings->pswd,  ptr, ES_WIFI_MAX_PSWD_NAME_SIZE + 1);
      break;

    case 2:
        NetSettings->Security = (ES_WIFI_SecurityType_t)ParseNumber(ptr, NULL);
        break;

    case 3:
      NetSettings->DHCP_IsEnabled = ParseNumber(ptr, NULL);
      break;

    case 4:
      NetSettings->IP_Ver = (ES_WIFI_IPVer_t)ParseNumber(ptr, NULL);
      break;

    case 5:
      ParseIP(ptr, NetSettings->IP_Addr);
      break;

    case 6:
      ParseIP(ptr, NetSettings->IP_Mask);
      break;

    case 7:
      ParseIP(ptr, NetSettings->Gateway_Addr);
      break;

    case 8:
      ParseIP(ptr, NetSettings->DNS1);
      break;

    case 9:
      ParseIP(ptr, NetSettings->DNS2);
      break;

    case 10:
      NetSettings->JoinRetries = ParseNumber(ptr, NULL);
      break;

    case 11:
      NetSettings->AutoConnect = ParseNumber(ptr, NULL);
      break;

    default:
      break;
    }
    ptr = strtok(NULL, ",");
    if ((ptr != NULL) && (ptr[-1] == ','))
    { /* Ignore empty fields */
      num++;
    }
  }
}

/**
  * @brief  Parses WIFI tr settings.
  * @param  pdata: pointer to data.
  * @param  ConnSettings: struct settings.
  * @retval None.
  */
void IsmDrvClass::AT_ParseTrSettings(char *pdata, ES_WIFI_Conn_t *ConnSettings)
{
  uint8_t num = 0;
  char *ptr;

  if((pdata == NULL) || (ConnSettings == NULL)) {
    return;
  }

  ptr = strtok(pdata + 2, ",");

  while (ptr != NULL) {
    switch (num++) {
    case 0:
      // Protocol identifier
      ptr++;
      break;

    case 1:
      ParseIP(ptr, ConnSettings->RemoteIP);
      break;

    case 2:
      ConnSettings->LocalPort = ParseNumber(ptr, NULL);
      break;

    case 3:
      // host IP;
      ptr++;
      break;

    case 4:
      ConnSettings->RemotePort = ParseNumber(ptr, NULL);
      break;

    default:
      break;
    }
    ptr = strtok(NULL, ",");
    if ((ptr != NULL) && (ptr[-1] == ','))
    { /* Ignore empty fields */
      num++;
    }
  }
}

/**
  * @brief  Execute AT command.
  * @param  cmd: pointer to command string
  * @param  pdata: pointer to returned data
  * @retval Operation Status.
  */
ES_WIFI_Status_t IsmDrvClass::AT_ExecuteCommand(uint8_t* cmd, uint8_t *pdata)
{
  int ret = 0;

  if((cmd == NULL) || (pdata == NULL)) {
    return ES_WIFI_STATUS_ERROR;
  }

  PRINTCMD(cmd);
  ret = Drv->IO_Send(cmd, strlen((char*)cmd), EsWifiObj.Timeout);
  if( ret > 0)
  {
    if(cmd == pdata) {
      memset(cmd, '\0', strlen((char*)cmd));
    }

    int16_t n=Drv->IO_Receive(pdata, ES_WIFI_DATA_SIZE, EsWifiObj.Timeout);
    if((n > 0) && (n < ES_WIFI_DATA_SIZE))
    {
      PRINTCMD(pdata);
      *(pdata+n)=0;
      if(strstr((char *)pdata, AT_OK_STRING))
      {
        return ES_WIFI_STATUS_OK;
      }
      else if(strstr((char *)pdata, AT_ERROR_STRING))
      {
        return ES_WIFI_STATUS_ERROR;
      }
    }
  }
  return ES_WIFI_STATUS_IO_ERROR;
}

/**
  * @brief  Execute AT command with data.
  * @param  cmd: pointer to command string
  * @param  pcmd_data: pointer to binary data
  * @param  len: binary data length
  * @param  pdata: pointer to returned data
  * @retval Operation Status.
  */
ES_WIFI_Status_t IsmDrvClass::AT_RequestSendData(uint8_t* cmd, 
uint16_t len, uint8_t *pdata)
{
  if((cmd == NULL) || (pdata == NULL)) {
    return ES_WIFI_STATUS_ERROR;
  }
  PRINTCMD(cmd);
  if(Drv->IO_Send(cmd, len, EsWifiObj.Timeout) > 0)
  {
    PRINTDATA(cmd, len);
    if(cmd == pdata) {
      memset(cmd, '\0', len);
    }

    int16_t n = Drv->IO_Receive(pdata, ES_WIFI_DATA_SIZE, EsWifiObj.Timeout);
    if((n > 0) && (n < ES_WIFI_DATA_SIZE))
    {
    PRINTCMD(pdata);
      *(pdata+n)=0;
      if(strstr((char *)pdata, AT_OK_STRING))
      {
        return ES_WIFI_STATUS_OK;
      }
      else if(strstr((char *)pdata, AT_ERROR_STRING))
      {
        return ES_WIFI_STATUS_ERROR;
      } else {
      }
    } else {
    }
  }
  return ES_WIFI_STATUS_IO_ERROR;
}


/**
  * @brief  Parses Received short data length.
  * @param  pdata: payload
  * @param  Reqlen : requested Data length.
  * @param  ReadData : pointer to received data length.
  * @retval Operation Status.
  */
ES_WIFI_Status_t IsmDrvClass::ReceiveShortDataLen(char *pdata, uint16_t Reqlen,
 uint16_t *ReadData)
{
   uint16_t len;

   if((ReadData == NULL) || (pdata == NULL)) {
     return ES_WIFI_STATUS_ERROR;
   }

   len = Drv->IO_Receive(EsWifiObj.CmdData, Reqlen + AT_OK_STRING_LEN , EsWifiObj.Timeout);

   if (len >= AT_OK_STRING_LEN)
   {
     EsWifiObj.CmdData[len] = '\0';
     if(strstr((char *)EsWifiObj.CmdData + len - AT_OK_STRING_LEN, AT_OK_STRING))
     {
       *ReadData = len - AT_OK_STRING_LEN;
       memcpy(pdata, EsWifiObj.CmdData, *ReadData);
       return ES_WIFI_STATUS_OK;
     }
     else
     {
       /* Drain the error message out of the input buffer. */
       len = Drv->IO_Receive(EsWifiObj.CmdData + len, sizeof(EsWifiObj.CmdData) - len - 1, EsWifiObj.Timeout);
       EsWifiObj.CmdData[len] = '\0';
     }
   }
   return ES_WIFI_STATUS_IO_ERROR;

}

/**
  * @brief  Parses Received long data length.
  * @param  pdata: payload
  * @param  Reqlen : requested Data length.
  * @param  ReadData : pointer to received data length.
  * @retval Operation Status.
  */
ES_WIFI_Status_t IsmDrvClass::ReceiveLongDataLen(char *pdata, uint16_t Reqlen,
 uint16_t *ReadData)
{
  uint16_t len = 0;
  uint16_t rlen = 0;
  uint16_t dlen = 0;

  if((ReadData == NULL) || (pdata == NULL)) {
    return ES_WIFI_STATUS_ERROR;
  }

  len = Drv->IO_Receive((uint8_t *)pdata, Reqlen, EsWifiObj.Timeout);

  if (len >= AT_OK_STRING_LEN)
  {
    PRINTDATA(pdata, len);
    /* strstr cannot be used here because the scanned buffer is not null-terminated and cannot be padded (it is owned by the caller). */
    if( (memcmp((char *)pdata + len - AT_OK_STRING_LEN, AT_OK_STRING, AT_OK_STRING_LEN) == 0)
       || (memcmp((char *)pdata + len - 1 - AT_OK_STRING_LEN, AT_OK_STRING, AT_OK_STRING_LEN) == 0) )  /* -1 offset in case of 16b alignement stuffing on the physical link */
    {
      *ReadData = len - AT_OK_STRING_LEN;
      return ES_WIFI_STATUS_OK;
    }
    else
    {
      /* Warn: The last (AT_OK_STRING_LEN - 1) bytes of the TCP payload will be scanned for AT_OK_STRING.
       *       This may result into a false positive detection. */
      memcpy(EsWifiObj.CmdData, pdata + len - AT_OK_STRING_LEN, AT_OK_STRING_LEN);
      rlen = Drv->IO_Receive(EsWifiObj.CmdData + AT_OK_STRING_LEN, AT_OK_STRING_LEN, EsWifiObj.Timeout);
      EsWifiObj.CmdData[AT_OK_STRING_LEN + rlen] = '\0';
      if(strstr((char *) EsWifiObj.CmdData + rlen, AT_OK_STRING))
      {
        *ReadData = len + rlen - AT_OK_STRING_LEN;
        return ES_WIFI_STATUS_OK;
      }
      else
      {
        /* Drain the error message out of the input buffer. */
        dlen = Drv->IO_Receive(EsWifiObj.CmdData + AT_OK_STRING_LEN + rlen, sizeof(EsWifiObj.CmdData) - AT_OK_STRING_LEN - rlen - 1, EsWifiObj.Timeout);
        EsWifiObj.CmdData[AT_OK_STRING_LEN + rlen + dlen] = '\0';
      }
    }
  }
  return ES_WIFI_STATUS_IO_ERROR;
}

/**
  * @brief  Parses Received data.
  * @param  cmd:command formatted string
  * @param  pdata: payload
  * @param  Reqlen : requested Data length.
  * @param  ReadData : pointer to received data length.
  * @retval Operation Status.
  */
ES_WIFI_Status_t IsmDrvClass::AT_RequestReceiveData(uint8_t* cmd,  char *pdata,
 uint16_t Reqlen, uint16_t *ReadData)
{
  if((cmd == NULL) || (pdata == NULL) || (ReadData == NULL) ) {
    return ES_WIFI_STATUS_ERROR;
  }

  PRINTCMD(cmd);
  if(Drv->IO_Send(cmd, strlen((char*)cmd), EsWifiObj.Timeout) > 0)
  {
    if(Drv->IO_Receive(EsWifiObj.CmdData, 2, EsWifiObj.Timeout) == 2) /* Read Prompt */
    {
      PRINTDATA(EsWifiObj.CmdData, 2);
      if (Reqlen <= AT_OK_STRING_LEN)
      {
        return ReceiveShortDataLen(pdata, Reqlen ,ReadData);
      }
      if(Reqlen >  AT_OK_STRING_LEN)
      {
        return ReceiveLongDataLen(pdata, Reqlen ,ReadData);
      }
    }
  }
  return ES_WIFI_STATUS_IO_ERROR;
}



/****************************function exported*********************************/

/**
  * @brief  Initialize WiFi module.
  * @param  None.
  * @retval Operation Status.
  */
ES_WIFI_Status_t IsmDrvClass::ES_WIFI_Init()
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;

  EsWifiObj.Timeout = ES_WIFI_TIMEOUT;

  if (Drv->IO_Init() == 0)
  {
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData, AT_API_SHOW_SETTINGS);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

    if(ret == ES_WIFI_STATUS_OK)
    {
      AT_ParseInfo (EsWifiObj.CmdData);
    }
  }
  return ret;
}

/**
  * @brief  Return ProductID.
  * @param  None
  * @retval ProductID.
  */
uint8_t* IsmDrvClass::ES_WIFI_GetProductID()
{
  return EsWifiObj.Product_ID;
}

/**
  * @brief  Return Firmware Revision.
  * @param  None
  * @retval pointer to the firmware version.
  */
char* IsmDrvClass::ES_WIFI_GetFWRevID()
{
  return (char *)EsWifiObj.FW_Rev;
}

/**
  * @brief  Return product Name.
  * @param  None.
  * @retval Product name.
  */
uint8_t* IsmDrvClass::ES_WIFI_GetProductName()
{
  return EsWifiObj.Product_Name;
}

/**
  * @brief  Return API revision.
  * @param  None.
  * @retval API revision.
  */
uint8_t* IsmDrvClass::ES_WIFI_GetAPIRev()
{
  return EsWifiObj.API_Rev;
}

/**
  * @brief  Return Stack Revision.
  * @param  None.
  * @retval Operation Status.
  */
uint8_t* IsmDrvClass::ES_WIFI_GetStackRev()
{
  return EsWifiObj.Stack_Rev;
}

/**
  * @brief  Return RTOS Revision
  * @param  None
  * @retval RTOS Revision
  */
uint8_t*   IsmDrvClass::ES_WIFI_GetRTOSRev()
{
  return EsWifiObj.RTOS_Rev;
}


/**
  * @brief  Change default Timeout.
  * @param  Timeout: Timeout in ms.
  * @retval Operation Status.
  */
ES_WIFI_Status_t  IsmDrvClass::ES_WIFI_SetTimeout(uint32_t Timeout)
{
  EsWifiObj.Timeout = Timeout;
  return ES_WIFI_STATUS_OK;
}

/**
  * @brief  List all detected APs.
  * @param  None.
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_ListAccessPoints()
{
  ES_WIFI_Status_t ret;

  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData, AT_SCAN);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
     AT_ParseAP((char *)EsWifiObj.CmdData, &ESWifiApObj);
  }
}

/**
  * @brief  Get the number of Acces point in view.
  * @param  None.
  * @retval Number of access point in view.
  */
int IsmDrvClass::ES_WIFI_GetApNbr()
{
  return ESWifiApObj.nbr;
}

/**
  * @brief  Join an Access point.
  * @param  Ssid: the access point id.
  * @param  Password: the Access point password.
  * @param  SecType: Security type.
  * @retval Operation Status.
  */
ES_WIFI_Status_t IsmDrvClass::ES_WIFI_Connect(const char* SSID,
const char* Password, ES_WIFI_SecurityType_t SecType)
{
  ES_WIFI_Status_t ret;
  char            CmdData[50];

  if((SSID == NULL) || (Password == NULL)) {
    return ES_WIFI_STATUS_ERROR;
  }

  sprintf(CmdData,"=%s", SSID);
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_NET_SET_SSID);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf(CmdData,"=%s", Password);
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_NET_SET_PASSPHRASE);
    strcat((char*)EsWifiObj.CmdData, CmdData);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

    if(ret == ES_WIFI_STATUS_OK)
    {
      EsWifiObj.Security = SecType;
      sprintf(CmdData,"=%d", SecType);
      memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
      strcpy((char*)EsWifiObj.CmdData,AT_NET_SET_SECURITY_TYPE);
      strcat((char*)EsWifiObj.CmdData, CmdData);
      strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
      ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);


      if(ret == ES_WIFI_STATUS_OK)
      {
        //Allows to wait the response in case of reconnection.
        // If less, then the response is missed and the reconnection is longer.
        ES_WIFI_SetTimeout(10000);
        memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
        strcpy((char*)EsWifiObj.CmdData,AT_NET_JOIN);
        strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
        ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {
           EsWifiObj.NetSettings.IsConnected = 1;
        }
        ES_WIFI_SetTimeout(ES_WIFI_TIMEOUT);
      }
    }
  }
  return ret;
}

/**
  * @brief  Disconnect from a network.
  * @param  None.
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_Disconnect()
{
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_NET_DISCONNECT);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
}

/**
  * @brief  Return network settings.
  * @param  Pointer to network setting structure.
  * @retval Operation Status.
  */
ES_WIFI_Status_t IsmDrvClass::ES_WIFI_GetNetworkSettings()
{
  ES_WIFI_Status_t ret;

  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_NET_SHOW_SETTINGS);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
     AT_ParseConnSettings((char *)EsWifiObj.CmdData, &EsWifiObj.NetSettings);
  }
  return ret;
}

/**
  * @brief  Return tr settings.
  * @param  index : socket number on which you want the settings
  * @retval Operation Status.
  */
ES_WIFI_Status_t IsmDrvClass::ES_WIFI_GetTrSettings(uint8_t index)
{
  ES_WIFI_Status_t ret;

  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_TR_SHOW_SETTNGS);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
     AT_ParseTrSettings((char *)EsWifiObj.CmdData, &ESWifiConnTab[index]);
  }
  return ret;
}


/**
  * @brief  return the MAC address of the module.
  * @param  pointer to the mac address.
  * @retval pointer to the mac address.
  */
uint8_t* IsmDrvClass::ES_WIFI_GetMACAddress(uint8_t *mac)
{
  ES_WIFI_Status_t ret ;
  char *ptr;

  if(mac == NULL) {
    return NULL;
  }

  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_SYS_GET_MAC_ADDR);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    ptr = strtok((char *)(EsWifiObj.CmdData + 2), "\r\n");
    ParseMAC(ptr, EsWifiObj.APSettings.MAC_Addr);
    for (int i=0; i<6; i++)
    {
      mac[i] = EsWifiObj.APSettings.MAC_Addr[i];
    }
  }
  return mac;
}

/**
  * @brief  return the IP address of the module.
  * @param  none.
  * @retval IP address.
  */
IPAddress IsmDrvClass::ES_WIFI_GetIPAddress()
{
  ES_WIFI_GetNetworkSettings();
  return EsWifiObj.NetSettings.IP_Addr;
}

/**
  * @brief  Get the interface subnet mask address.
  * @param  none.
  * @retval submask.
  */
IPAddress IsmDrvClass::ES_WIFI_GetSubnetMask()
{
  ES_WIFI_GetNetworkSettings();
  return EsWifiObj.NetSettings.IP_Mask;
}

/**
  * @brief  Get the gateway ip address.
  * @param  none.
  * @retval gateway IP.
  */
IPAddress IsmDrvClass::ES_WIFI_GetGatewayIP()
{
  ES_WIFI_GetNetworkSettings();
  return EsWifiObj.NetSettings.Gateway_Addr;
}

 /*
 * Return the current SSID associated with the network
 * @param : None
 * return: pointer to SSID
 */
uint8_t* IsmDrvClass::ES_WIFI_GetSSID()
{
  ES_WIFI_GetNetworkSettings();
  return EsWifiObj.NetSettings.SSID;
}

 /*
 * Return the SSID discovered during the network scan.
 * @param networkItem: specify from which network item want to get the information
 * return: ssid string of the specified item on the networks scanned list
 * Note : ES_WIFI_ListAccessPoints() should be perform before call this function
 */
uint8_t* IsmDrvClass::ES_WIFI_GetSSID(uint8_t networkItem)
{
  // List all access point in view
  for (int j=0; j<ESWifiApObj.nbr; j++)
  {
    // Find our access point
    if (j == networkItem)
    {
      // return SSID
      return ESWifiApObj.AP[j].SSID;
    }
  }
  return NULL;
}

 /*
 * Return the current BSSID associated with the network
 * @param : pointer to BBSID
 * return: pointer to BBSID
 */
uint8_t* IsmDrvClass::ES_WIFI_GetBSSID(uint8_t *bssid)
{
  if(bssid == NULL) {
    return NULL;
  }

  ES_WIFI_ListAccessPoints();
  // List all access point in view
  for (int j=0; j<ESWifiApObj.nbr; j++)
  {
    // Find our access point
    if (strcmp((char*)ESWifiApObj.AP[j].SSID, (char*)EsWifiObj.NetSettings.SSID) == 0)
    {
      // cpy mac address of the access point
      for (int i=0; i<6; i++)
      {
      bssid[i] = ESWifiApObj.AP[j].MAC[i];
      }
      break;
    }
  }
  return bssid;
}

 /*
 * Return the Encryption Type associated with the network
 * @param : None
 * return: one value of ES_WIFI_SecurityType_t enum
 */
int IsmDrvClass::ES_WIFI_GetEncryptionType()
{
  ES_WIFI_GetNetworkSettings();
  return EsWifiObj.NetSettings.Security;
}

 /*
 * Return the Encryption Type associated with the network
 * @param networkItem: specify from which network item want to get the information
 * return: one value of ES_WIFI_SecurityType_t enum
 * Note : ES_WIFI_ListAccessPoints() should be perform before call this function
 */
int IsmDrvClass::ES_WIFI_GetEncryptionType(uint8_t networkItem)
{
  // List all access point in view
  for (int j=0; j<ESWifiApObj.nbr; j++)
  {
    // Find our access point
    if (j == networkItem)
    {
      // return RSSI
      return ESWifiApObj.AP[j].Security;
    }
  }
  return 0;
}

 /*
 * Return the RSSI of the associate Access point
 * @param : None
 * return: rssi
 */
int32_t IsmDrvClass::ES_WIFI_GetRSSI()
{
  ES_WIFI_ListAccessPoints();
  // List all access point in view
  for (int j=0; j<ESWifiApObj.nbr; j++)
  {
    // Find our access point
    if (strcmp((char*)ESWifiApObj.AP[j].SSID, (char*)EsWifiObj.NetSettings.SSID) == 0)
    {
      // return RSSI
      return ESWifiApObj.AP[j].RSSI;
    }
  }
  return 0;
}

 /*
 * Return the RSSI of a specific access point
 * @param networkItem: specify from which network item want to get the information
 * return: rssi
 * Note : ES_WIFI_ListAccessPoints() should be perform before call this function
 */
int32_t IsmDrvClass::ES_WIFI_GetRSSI(uint8_t networkItem)
{
  // List all access point in view
  for (int j=0; j<ESWifiApObj.nbr; j++)
  {
    // Find our access point
    if (j == networkItem)
    {
      // return RSSI
      return ESWifiApObj.AP[j].RSSI;
    }
  }
  return 0;
}

/**
  * @brief  Set the MAC address of the module.
  * @param  mac: the MAC address to set.
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_SetMACAddress(uint8_t *mac)
{
  ES_WIFI_Status_t ret ;
  char            CmdData[50];

  if(mac == NULL) {
    return;
  }

  sprintf(CmdData,"=%X:%X:%X:%X:%X:%X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_SYS_SET_MAC_ADDR);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_SYS_SAVE_SETTINGS);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  }
}

/**
  * @brief  Reset To factory defaults.
  * @param  None.
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_ResetToFactoryDefault()
{
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_SYS_SET_RESET_FACTORY);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
}

/**
  * @brief  Reset the module.
  * @param  None.
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_ResetModule()
{
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_SYS_RESET_MODULE);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
}

/**
  * @brief  Set Product Name.
  * @param  ProductName : pointer to product name string
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_SetProductName(uint8_t *ProductName)
{
  ES_WIFI_Status_t ret ;
  char            CmdData[50];

  if(ProductName == NULL) {
    return;
  }

  sprintf(CmdData,"=%s", ProductName);
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_SYS_SET_PRODUCT_NAME);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_SYS_SAVE_SETTINGS);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  }
}

/**
  * @brief  Get System Configuration.
  * @param  None.
  * @retval Operation Status.
  */
ES_WIFI_Status_t IsmDrvClass::ES_WIFI_GetSystemConfig()
{
  ES_WIFI_Status_t ret ;

  sprintf((char*)EsWifiObj.CmdData,"Z?\r");
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    AT_ParseSystemConfig((char *)EsWifiObj.CmdData, &ESWifiSysObj);
  }
  return ret;
}

/**
  * @brief  Ping an IP address.
  * @param  address: address to ping.
  * @param  count: number of ping to do.
  * @param  interval_ms: time between ping.
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_Ping(uint8_t *address, uint16_t count, uint16_t interval_ms)
{
  ES_WIFI_Status_t ret;
  char            CmdData[50];

  if(address == NULL) {
    return;
  }

  sprintf(CmdData,"=%d.%d.%d.%d",  address[0],address[1], address[2],address[3]);
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_PING_SET_TARGET_ADDR);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf(CmdData,"=%d", count);
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_PING_SET_REPEAT_COUNT);
    strcat((char*)EsWifiObj.CmdData, CmdData);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

    if(ret == ES_WIFI_STATUS_OK)
    {
      sprintf(CmdData,"=%d", interval_ms);
      memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
      strcpy((char*)EsWifiObj.CmdData,AT_PING_SET_DELAY_MS);
      strcat((char*)EsWifiObj.CmdData, CmdData);
      strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
      ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

      if(ret == ES_WIFI_STATUS_OK)
      {
        memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
        strcpy((char*)EsWifiObj.CmdData,AT_PING_TARGET_ADDR);
        strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
        ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
      }
    }
  }
}

/**
  * @brief  DNS Lookup to get IP address .
  * @param  url: Domain Name.
  * @param  ipaddress: IP address.
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_DNS_LookUp(const char *url, IPAddress *ipaddress)
{
  ES_WIFI_Status_t ret;
  char *ptr;
  char            CmdData[50];
  uint8_t temp[4];

  if((url == NULL) || (ipaddress == NULL)) {
    return;
  }

  sprintf(CmdData,"=%s", url);
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_DNS_LOOKUP);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
    ptr = strtok((char *)EsWifiObj.CmdData + 2, "\r");
    ParseIP(ptr, temp);
    *ipaddress = IPAddress(temp);
  }
}


/**
  * @brief  Configure and Start a Client connection.
  * @param  index : index of structure connection
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_StartClientConnection(uint8_t index)
{
  ES_WIFI_Status_t ret;
  char            CmdData[50];

  currentSock = index;
  sockState[index] = SOCKET_BUSY;
  sprintf(CmdData,"=%d", ESWifiConnTab[index].Number);
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_SOCKET);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf(CmdData,"=%d", ESWifiConnTab[index].Type);
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_PROTOCOL);
    strcat((char*)EsWifiObj.CmdData, CmdData);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
      if ((ESWifiConnTab[index].Type == ES_WIFI_UDP_CONNECTION) && (ESWifiConnTab[index].LocalPort > 0))
      {
        sprintf(CmdData,"=%d", ESWifiConnTab[index].LocalPort);
        memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
        strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_LOCAL_PORT_NUMBER);
        strcat((char*)EsWifiObj.CmdData, CmdData);
        strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
        if(AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData) == ES_WIFI_STATUS_ERROR)
        {
          ret = ES_WIFI_STATUS_ERROR;
        }
      }
      sprintf(CmdData,"=%d", ESWifiConnTab[index].RemotePort);
      memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
      strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_REMOTE_PORT_NUMBER);
      strcat((char*)EsWifiObj.CmdData, CmdData);
      strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
      ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

      if(ret == ES_WIFI_STATUS_OK)
      {
        sprintf(CmdData,"=%d.%d.%d.%d", ESWifiConnTab[index].RemoteIP[0],ESWifiConnTab[index].RemoteIP[1],
                ESWifiConnTab[index].RemoteIP[2],ESWifiConnTab[index].RemoteIP[3]);
        memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
        strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_REMOTE_HOST_IP_ADDR);
        strcat((char*)EsWifiObj.CmdData, CmdData);
        strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
        ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

        if(ret == ES_WIFI_STATUS_OK)
        {
          sprintf(CmdData,"=1");
          memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
          strcpy((char*)EsWifiObj.CmdData,AT_TR_CLIENT);
          strcat((char*)EsWifiObj.CmdData, CmdData);
          strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
          ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
        }
      }
    }
  }
}

/**
  * @brief  Stop Client connection.
  * @param  index : index of structure connection
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_StopClientConnection(uint8_t index)
{
  ES_WIFI_Status_t ret;
  char            CmdData[50];

  currentSock = index;
  sockState[currentSock] = SOCKET_FREE;
  sprintf(CmdData,"=%d",index);
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_SOCKET);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
    sprintf(CmdData,"=0");
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_TR_CLIENT);
    strcat((char*)EsWifiObj.CmdData, CmdData);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret =  AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  }
}

/**
  * @brief  Configure and Start a Server.
  * @param  index  : index of structure connection
  * @param  mode   : communication mode
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_StartServerSingleConn(uint8_t index, comm_mode mode)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  char *ptr;
  char            CmdData[50];

  if (index > MAX_SOCK_NUM)
  {
    return;
  }

  sprintf(CmdData,"=1,3000");
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_TR_TCP_KEEP_ALIVE);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    currentSock = index;
    sockState[currentSock] = SOCKET_BUSY;
    sprintf(CmdData,"=%d", ESWifiConnTab[index].Number);
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_SOCKET);
    strcat((char*)EsWifiObj.CmdData, CmdData);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
      sprintf(CmdData,"=%d", ESWifiConnTab[index].Type);
      memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
      strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_PROTOCOL);
      strcat((char*)EsWifiObj.CmdData, CmdData);
      strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
      ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
      if(ret == ES_WIFI_STATUS_OK)
      {
        sprintf(CmdData,"=%d", ESWifiConnTab[index].LocalPort);
        memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
        strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_LOCAL_PORT_NUMBER);
        strcat((char*)EsWifiObj.CmdData, CmdData);
        strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
        ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {
          sprintf(CmdData,"=1");
          memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
          strcpy((char*)EsWifiObj.CmdData,AT_TR_SERVER);
          strcat((char*)EsWifiObj.CmdData, CmdData);
          strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
          ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

          if(ret == ES_WIFI_STATUS_OK)
          {
            if(mode == COMM_UART)
            {
              if(Drv->IO_Receive(EsWifiObj.CmdData, 0, EsWifiObj.Timeout) > 0)
              {
                if(strstr((char *)EsWifiObj.CmdData, "Accepted"))
                {
                  ptr = strtok((char *)EsWifiObj.CmdData + 2, " ");
                  ptr = strtok(NULL, " ");
                  ptr = strtok(NULL, " ");
                  ptr = strtok(NULL, ":");
                  ParseIP((char *)ptr, ESWifiConnTab[index].RemoteIP);
                  ret = ES_WIFI_STATUS_OK;
                }
              }
            }
            else if (mode == COMM_SPI)
            {
              do
              {
                memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
                strcpy((char*)EsWifiObj.CmdData,AT_MESSAGE_READ);
                strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
                ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
                if(ret == ES_WIFI_STATUS_OK)
                {
                  if((strstr((char *)EsWifiObj.CmdData, "[SOMA]")) && (strstr((char *)EsWifiObj.CmdData, "[EOMA]")))
                  {
                    if(strstr((char *)EsWifiObj.CmdData, "Accepted"))
                    {
                      ptr = strtok((char *)EsWifiObj.CmdData + 2, " ");
                      ptr = strtok(NULL, " ");
                      ptr = strtok(NULL, " ");
                      ptr = strtok(NULL, ":");
                      ParseIP((char *)ptr, ESWifiConnTab[index].RemoteIP);
                      ret = ES_WIFI_STATUS_OK;
                      break;
                    }
                  }
                }
                else
                {
                  ret = ES_WIFI_STATUS_ERROR;
                  break;
                }
                Drv->IO_Delay(1000);
              } while (1);
            }
            else
            {
              ret = ES_WIFI_STATUS_ERROR;
            }
          }
        }
      }
    }
  }
}

/**
  * @brief  Stop a Server.
  * @param index : socket to free
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_StopServerSingleConn(uint8_t index)
{
  char            CmdData[50];
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;

  currentSock = index;
  sockState[currentSock] = SOCKET_FREE;
  sprintf(CmdData,"=%d",index);
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_SOCKET);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  if (ret == ES_WIFI_STATUS_OK)
  {
    sprintf(CmdData,"=0");
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_TR_SERVER);
    strcat((char*)EsWifiObj.CmdData, CmdData);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  }
}


/**
  * @brief  Configure and Start a Server.
  * @param index : socket to use
  * @param  mode : communication mode
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_StartServerMultiConn(uint8_t index, comm_mode mode)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  char *ptr;
  char            CmdData[50];

  sprintf(CmdData,"=1,3000");
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_TR_TCP_KEEP_ALIVE);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
  if(ret == ES_WIFI_STATUS_OK)
  {
    currentSock = index;
    sockState[currentSock] = SOCKET_BUSY;
    sprintf(CmdData,"=%d", ESWifiConnTab[index].Number);
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_SOCKET);
    strcat((char*)EsWifiObj.CmdData, CmdData);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
      sprintf(CmdData,"=%d", ESWifiConnTab[index].Type);
      memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
      strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_PROTOCOL);
      strcat((char*)EsWifiObj.CmdData, CmdData);
      strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
      ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
      if(ret == ES_WIFI_STATUS_OK)
      {
        sprintf(CmdData,"=%d", ESWifiConnTab[index].LocalPort);
        memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
        strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_LOCAL_PORT_NUMBER);
        strcat((char*)EsWifiObj.CmdData, CmdData);
        strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
        ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {
          sprintf(CmdData,"=6");
          memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
          strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_LISTEN_BACKLOGS);
          strcat((char*)EsWifiObj.CmdData, CmdData);
          strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
          ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

          if(ret == ES_WIFI_STATUS_OK)
          {
            sprintf(CmdData,"=1");
            memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
            strcpy((char*)EsWifiObj.CmdData,AT_TR_SERVER);
            strcat((char*)EsWifiObj.CmdData, CmdData);
            strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
            ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

            if(ret == ES_WIFI_STATUS_OK)
            {
              if (mode == COMM_UART)
              {
                if(Drv->IO_Receive(EsWifiObj.CmdData, 0, EsWifiObj.Timeout) > 0)
                {
                  if(strstr((char *)EsWifiObj.CmdData, "Accepted"))
                  {
                    ptr = strtok((char *)EsWifiObj.CmdData + 2, " ");
                    ptr = strtok(NULL, " ");
                    ptr = strtok(NULL, " ");
                    ptr = strtok(NULL, ":");
                    ParseIP((char *)ptr, ESWifiConnTab[index].RemoteIP);
                    ret = ES_WIFI_STATUS_OK;
                  }
                }
              }
              else if (mode == COMM_SPI)
              {
                do
                {
                  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
                  strcpy((char*)EsWifiObj.CmdData,AT_MESSAGE_READ);
                  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
                  ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
                  if(ret == ES_WIFI_STATUS_OK)
                  {
                    if((strstr((char *)EsWifiObj.CmdData, "[SOMA]")) && (strstr((char *)EsWifiObj.CmdData, "[EOMA]")))
                    {
                      if(strstr((char *)EsWifiObj.CmdData, "Accepted"))
                      {
                        ptr = strtok((char *)EsWifiObj.CmdData + 2, " ");
                        ptr = strtok(NULL, " ");
                        ptr = strtok(NULL, " ");
                        ptr = strtok(NULL, ":");
                        ParseIP((char *)ptr, ESWifiConnTab[index].RemoteIP);
                        ret = ES_WIFI_STATUS_OK;
                        break;
                      }
                    }
                  }
                  else
                  {
                    ret = ES_WIFI_STATUS_ERROR;
                    break;
                  }
                  Drv->IO_Delay(1000);
                } while (1);
              }
              else
              {
                ret = ES_WIFI_STATUS_ERROR;
              }
            }
            if(ret == ES_WIFI_STATUS_OK)
            {
              sprintf(CmdData,"=1");
              memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
              strcpy((char*)EsWifiObj.CmdData,AT_TR_REQUEST_TCP_LOOP);
              strcat((char*)EsWifiObj.CmdData, CmdData);
              strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
              ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

            }
          }
        }
      }
    }
  }
}

/**
  * @brief  Stop a Server.
  * @param  None.
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_StopServerMultiConn()
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  char            CmdData[50];

  /* close the socket handle for the current request. */
  sprintf(CmdData,"=2");
  memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
  strcpy((char*)EsWifiObj.CmdData,AT_TR_REQUEST_TCP_LOOP);
  strcat((char*)EsWifiObj.CmdData, CmdData);
  strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
  ret =  AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

  if(ret == ES_WIFI_STATUS_OK)
  {
    /*Get the next request out of the queue */
    sprintf(CmdData,"=3");
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_TR_REQUEST_TCP_LOOP);
    strcat((char*)EsWifiObj.CmdData, CmdData);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
      if(ret == ES_WIFI_STATUS_OK)
      {
        if(Drv->IO_Receive(EsWifiObj.CmdData, 0, EsWifiObj.Timeout) > 0)
        {
          if(strstr((char *)EsWifiObj.CmdData, "Accepted"))
          {
            ret = ES_WIFI_STATUS_OK;
          }
        }
      }
    }
  }
}

/**
  * @brief  Send an amount data over WIFI.
  * @param  Socket  : number of the socket
  * @param  pdata   : pointer to data
  * @param  len     : length of the data to be sent
  * @param  SentLen : length of data really send
  * @param  Timeout : Timeout for sending data in ms (range of 0 to 30000)
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_SendResp(uint8_t Socket, uint8_t *pdata,
                 uint16_t Reqlen , uint16_t *SentLen , uint32_t Timeout)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  char            CmdData[50];

  if((pdata == NULL) || (SentLen == NULL)) {
    return;
  }

  if(Reqlen <= ES_WIFI_PAYLOAD_SIZE )
  {
    currentSock = Socket;
    sprintf(CmdData,"=%d", Socket);
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_SOCKET);
    strcat((char*)EsWifiObj.CmdData, CmdData);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
    if(ret == ES_WIFI_STATUS_OK)
    {
      ES_WIFI_SetTimeout(Timeout);
      // Timeout for the device must be shorter than our timeout otherwise
      // the device may answer to late.
      sprintf(CmdData,"=%lu", Timeout-TIMEOUT_OFFSET);
      memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
      strcpy((char*)EsWifiObj.CmdData, AT_WRITE_SET_TIMEOUT);
      strcat((char*)EsWifiObj.CmdData, CmdData);
      strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
      ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
      if(ret == ES_WIFI_STATUS_OK)
      {
        sprintf(CmdData,"=%u", Reqlen);
        memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
        strcpy((char*)EsWifiObj.CmdData,AT_WRITE_SET_PACKET_SIZE);
        strcat((char*)EsWifiObj.CmdData, CmdData);
        strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
        ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {
          memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
          strcpy((char*)EsWifiObj.CmdData, AT_WRITE_DATA);
          strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
          memcpy((char*)EsWifiObj.CmdData + 3, pdata, Reqlen);
          ret = AT_RequestSendData(EsWifiObj.CmdData, Reqlen+3, EsWifiObj.CmdData);
          if(ret == ES_WIFI_STATUS_OK)
          {
            *SentLen = Reqlen;
          }
          else
          {
            *SentLen = 0;
          }
        }
      }
    }
  }
}

/**
  * @brief  Receive an amount data over WIFI.
  * @param  Socket      : number of the socket
  * @param  pdata       : pointer to data
  * @param  Reqlen      : pointer to the length of the data to be recieved
  * @param  Recievedlen : Number of bata really recieved
  * @param  timeout     : Timeout for recieving the data
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_ReceiveData(uint8_t Socket, uint8_t *pdata,
                       uint16_t Reqlen, uint16_t *Receivedlen, uint32_t Timeout)
{
  ES_WIFI_Status_t ret = ES_WIFI_STATUS_ERROR;
  char            CmdData[50];

  if((pdata == NULL) || (Receivedlen == NULL)) {
    return;
  }

  if(Reqlen <= ES_WIFI_PAYLOAD_SIZE )
  {
    currentSock = Socket;
    sprintf(CmdData,"=%d", Socket);
    memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
    strcpy((char*)EsWifiObj.CmdData,AT_TR_SET_SOCKET);
    strcat((char*)EsWifiObj.CmdData, CmdData);
    strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
    ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);

    if(ret == ES_WIFI_STATUS_OK)
    {
      sprintf(CmdData,"=%d", Reqlen);
      memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
      strcpy((char*)EsWifiObj.CmdData,AT_READ_SET_PACKET_SIZE);
      strcat((char*)EsWifiObj.CmdData, CmdData);
      strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
      ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
      if(ret == ES_WIFI_STATUS_OK)
      {
        // Timeout for the device must be shorter than our timeout otherwise
        // the device may answer to late.
        ES_WIFI_SetTimeout(Timeout);
        sprintf(CmdData,"=%lu", Timeout-TIMEOUT_OFFSET);
        memset(EsWifiObj.CmdData, '\0', ES_WIFI_DATA_SIZE);
        strcpy((char*)EsWifiObj.CmdData,AT_READ_SET_TIMEOUT_MS);
        strcat((char*)EsWifiObj.CmdData, CmdData);
        strcat((char*)EsWifiObj.CmdData, SUFFIX_CMD);
        ret = AT_ExecuteCommand(EsWifiObj.CmdData, EsWifiObj.CmdData);
        if(ret == ES_WIFI_STATUS_OK)
        {
          sprintf((char*)EsWifiObj.CmdData,AT_READ_DATA);
          memset(pdata, '\0', Reqlen);
          ret = AT_RequestReceiveData(EsWifiObj.CmdData, (char *)pdata, Reqlen, Receivedlen);
          if(strstr((char *)pdata,"-1\r\n"))
          {
            sockState[Socket] = SOCKET_FREE;
            *Receivedlen = 0;
          }
        }
      }
      else
      {
        *Receivedlen = 0;
      }
    }
  }
}

/**
  * @brief  Set connection parameter in the struct
  * @param  Number    : socket number
  * @param  Type      : Type of connection (UDP, TCP)
  * @param  LocalPort : local port
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_SetConnectionParam(uint8_t Number, ES_WIFI_ConnType_t Type, uint16_t LocalPort)
{
  if (Number < MAX_SOCK_NUM)
  {
    ESWifiConnTab[Number].Number = Number;
    ESWifiConnTab[Number].Type = Type;
    ESWifiConnTab[Number].LocalPort = LocalPort;
  }
}

/**
  * @brief  Set connection parameter in the struct
  * @param  Number    : socket number
  * @param  Type      : Type of connection (UDP, TCP)
  * @param  LocalPort : local port
  * @param  Ip        : Remote IP address
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_SetConnectionParam(uint8_t Number, ES_WIFI_ConnType_t Type, uint16_t LocalPort, IPAddress Ip)
{
  if (Number < MAX_SOCK_NUM)
  {
    ESWifiConnTab[Number].Number = Number;
    ESWifiConnTab[Number].Type = Type;
    ESWifiConnTab[Number].RemotePort = LocalPort;
    ESWifiConnTab[Number].LocalPort = LocalPort;
    for (int i=0; i<4; i++)
    {
      ESWifiConnTab[Number].RemoteIP[i] = Ip[i];
    }
  }
}

/**
  * @brief  Get remote Data information on UDP socket
  * @param  sock: socket identifier
  * @param  ip: remote IP
  * @param  port : remote port
  * @retval None.
  */
void IsmDrvClass::ES_WIFI_getRemoteData(uint8_t sock, uint8_t *ip, uint16_t *port)
{
  if((ip == NULL) || (port == NULL)) {
    return;
  }

  if (sock < MAX_SOCK_NUM)
  {
    ES_WIFI_GetTrSettings(sock);
    *port = ESWifiConnTab[sock].RemotePort;
    for (int i=0; i<4; i++)
    {
      ip[i] = ESWifiConnTab[sock].RemoteIP[i];
    }
  }
}


/**
  * @brief  Get the currrent socket use
  * @param  None
  * @retval socket number.
  */
uint8_t IsmDrvClass::getCurrentSocket(void)
{
  return currentSock;
}

/**
  * @brief  Get the first socket number free.
  * @param  None
  * @retval socket number or -1 if no available.
  */
int8_t IsmDrvClass::getFreeSocket(void)
{
  for(uint8_t i = 0; i < MAX_SOCK_NUM; i++) {
    if(sockState[i] == SOCKET_FREE) {
      return i;
    }
  }
  return -1;
}

/**
  * @brief  Get the socket state.
  * @param  socket: socket number
  * @retval the state of the socket: SOCKET_FREE or SOCKET_BUSY
  */
uint8_t IsmDrvClass::getSocketState(uint8_t socket)
{
  if(socket < MAX_SOCK_NUM)
    return (uint8_t)sockState[socket];
  else
    return (uint8_t)SOCKET_BUSY;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/ 
