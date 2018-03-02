/**
  ******************************************************************************
  * @file    spi_drv.cpp
  * @author  MCD Application Team, Wi6labs
  * @brief   This file implments the IO operations to deal with the es-wifi
  *          module. It mainly Inits and Deinits the SPI interface. Send and
  *          receive data over it.
  * original file in STM32Cube_FW_L4_V1.10.0, file es_wifi_io.c
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


#include <SPI.h>
#include <stdbool.h>
#include <string.h>
#include "Arduino.h"
#include "spi_drv.h"
#include "pins_arduino.h"

/**
 * @brief  Constructor.
 * @param  SPIx   : SPI interface
 * @param  cs     : chip select pin
 * @param  spiIRQ : SPI IRQ pin
 * @param  reset  : reset pin
 * @param  wakeup : wakeup pin
 * @retval None
 */
SpiDrvClass::SpiDrvClass(SPIClass *SPIx, uint8_t cs, uint8_t spiIRQ,
                         uint8_t reset, uint8_t wakeup) {
  ISM43362 = SPIx;
  csPin = cs;
  spiIRQPin = spiIRQ;
  wakeupPin = wakeup;
  resetPin = reset;
}

/**
 * @brief  Select the device.
 * @param  None
 * @retval None
 */
void SpiDrvClass::Spi_Slave_Select() {
    digitalWrite(csPin,LOW);
    delay(10);
}

/**
 * @brief  Unselect the device.
 * @param  None
 * @retval None
 */
void SpiDrvClass::Spi_Slave_Deselect() {
    digitalWrite(csPin,HIGH);
    delay(10);
}

/**
 * @brief  Get data ready state.
 * @param  None
 * @retval data_ready pin state. This is the spiIRQ pin.
 */
uint8_t SpiDrvClass::Spi_Get_Data_Ready_State()
{
  return digitalRead(spiIRQPin);
}

/**
 * @brief  Reset the WiFi device.
 * @param  None
 * @retval None
 */
void SpiDrvClass::Spi_Wifi_Reset() {
  digitalWrite(resetPin,LOW);
  delay(10);
  digitalWrite(resetPin, HIGH);
  delay(500);
}

/**
 * @brief  Initialize the SPI
 * @param  None
 * @retval : 0 if init success, -1 otherwise.
 */
int8_t SpiDrvClass::IO_Init(void)
{
  uint8_t Prompt[6];                     // data receive
  uint8_t count = 0;                     // number of bytes receive
  uint32_t start;                        // start time for timeout
  uint16_t dummy_send = 0x0A0A;          // data to feed the clock
  uint16_t read_value;                   // data read

  /* pin configuration                                                       */
  pinMode(wakeupPin, OUTPUT);
  digitalWrite(wakeupPin, LOW);

  pinMode(spiIRQPin, INPUT);

  pinMode(resetPin, OUTPUT);

  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);

  /* Clock = 10MHz (Inventek Wi-Fi module supports up to 20MHz)               */
  Settings_43362 = new SPISettings(10000000, MSBFIRST, SPI_MODE0);
  ISM43362->beginTransaction(*Settings_43362);

  Spi_Wifi_Reset();

  Spi_Slave_Select();

  start = millis();

  while (Spi_Get_Data_Ready_State())
  {
    read_value = ISM43362->transfer16(dummy_send);
    Prompt[count] = (uint8_t)(read_value & 0x00FF);
    Prompt[count+1] = (uint8_t)((read_value & 0xFF00) >> 8);
    count+=2;
    read_value = ISM43362->transfer16(dummy_send);
    Prompt[count] = (uint8_t)(read_value & 0x00FF);
    Prompt[count+1] = (uint8_t)((read_value & 0xFF00) >> 8);
    count+=2;
    read_value = ISM43362->transfer16(dummy_send);
    Prompt[count] = (uint8_t)(read_value & 0x00FF);
    Prompt[count+1] = (uint8_t)((read_value & 0xFF00) >> 8);
    if((millis() - start ) > 100)
    {
      Spi_Slave_Deselect();
      printf("timeout io_init\n\r");
      return -1;
    }
  }
  // Check receive sequence
  if((Prompt[0] != 0x15) ||(Prompt[1] != 0x15) ||(Prompt[2] != '\r')||
       (Prompt[3] != '\n') ||(Prompt[4] != '>') ||(Prompt[5] != ' '))
  {
    Spi_Slave_Deselect();
    return -1;
  }

  Spi_Slave_Deselect();
  return 0;
}

/**
 * @brief  DeInitialize the SPI
 * @param  None
 * @retval None
 */
void SpiDrvClass::IO_DeInit(void)
{
  ISM43362->endTransaction();
  ISM43362->end();
}

/**
 * @brief  Delay
 * @param  Delay in ms
 * @retval None
 */
void SpiDrvClass::IO_Delay(uint32_t time)
{
  delay(time);
}

/**
 * @brief  Send Wi-Fi Data through SPI
 * @param  pdata   : pointer to data
 * @param  len     : Data length in byte
 * @param  timeout : send timeout in ms
 * @retval Length of sent data, -1 if send fail
 */
int16_t SpiDrvClass::IO_Send( uint8_t *pdata,  uint16_t len, uint32_t timeout)
{
  uint8_t Padding[2];              // padding data
  uint32_t start;                  // start time for timeout
  int data_tx=0;                   // data really send
  uint16_t data_send;              // data to send

  start = millis();

  // Wait device ready to receive data
  while (!Spi_Get_Data_Ready_State())
  {
    if((millis() - start ) > timeout)
    {
       Spi_Slave_Deselect();
      return -1;
    }
  }

  // Send data
  Spi_Slave_Select();
  for (data_tx=0; data_tx<len; data_tx+=2)
  {
    if (data_tx == len-1)
    {
      // Data to send are odd, need padding
      Padding[0] = pdata[len-1];
      Padding[1] = '\n';
      data_send = Padding[0] | (Padding[1] << 8);
      ISM43362->transfer16(data_send);
    }
    else
    {
      data_send = pdata[data_tx] | (pdata[data_tx+1] << 8);
      ISM43362->transfer16(data_send);
    }
  }
  return data_tx;
}

/**
 * @brief  Receive Wi-Fi Data from SPI
 * @param  pdata   : pointer to data
 * @param  len     : Data length i byte
 * @param  timeout : send timeout in mS
 * @retval Length of received data (payload)
 */
int16_t SpiDrvClass::IO_Receive(uint8_t *pData, uint16_t len, uint32_t timeout)
{
  int16_t length = 0;                 // length of data receive
  uint8_t tmp[2];                     // 2 bytes buffer of receive
  uint32_t start;                     // start time for timeout
  uint16_t dummy_send = 0x0A0A;       // data to feed the clock
  uint16_t read_value;                // data read

  start = millis();

  Spi_Slave_Deselect();

  // Wait device reports that it has data to send
  while(!Spi_Get_Data_Ready_State())
  {
    if ((millis() - start) >= timeout)
    {
      return 0;
    }
  }

  // Receive device data
  Spi_Slave_Select();
  start = millis();
  while (Spi_Get_Data_Ready_State())
  {
    if((length < len) || (!len))
    {
     read_value = ISM43362->transfer16(dummy_send);
     tmp[0] = (uint8_t)(read_value & 0x00FF);
     tmp[1] = (uint8_t)((read_value & 0xFF00) >> 8);

      /* let some time to hardware to change data ready signal (the IRQpin) */
      if(tmp[1] == 0x15)
      {
       IO_Delay(1);
      }
      /*This the last data */
      if(!Spi_Get_Data_Ready_State())
      {
        if(tmp[1] == 0x15)
        {
          // Only 1 byte of data, the other one is padding
          if ((tmp[0] != 0x15))
          {
            pData[0] = tmp[0];
            length++;
          }
          break;
        }
      }

      pData[0] = tmp[0];
      pData[1] = tmp[1];
      length += 2;
      pData  += 2;

      if((millis() - start) >= timeout)
      {
        Spi_Slave_Deselect();
        return 0;
      }
    }
    else
    {
      break;
    }
  }

  Spi_Slave_Deselect();
  return length;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/    
