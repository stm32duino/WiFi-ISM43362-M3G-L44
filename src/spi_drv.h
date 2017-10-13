/**
  ******************************************************************************
  * @file    spi_drv.h
  * @author  MCD Application Team, Wi6labs
  * @brief   This file contains the functions prototypes for es_wifi IO operations.
  * original file in STM32Cube_FW_L4_V1.10.0, file es_wifi_io.h
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#ifndef SPI_DRV_H
#define SPI_DRV_H

#include <inttypes.h>
#include <SPI.h>
#include "driver.h"

#define MAX_TRY_INIT    100
#define MAX_SIZE_ANSWER 255

class SpiDrvClass : public DriverClass{

private :
  SPIClass *ISM43362;
  uint8_t csPin;
  SPISettings *Settings_43362;
  uint8_t spiIRQPin;

public:

  SpiDrvClass(SPIClass *SPIx, uint8_t cs, uint8_t spiIRQ, uint8_t reset, uint8_t wakeup);

  void Spi_Wifi_Reset();
  void Spi_Slave_Select();
  void Spi_Slave_Deselect();
  uint8_t Spi_Get_Data_Ready_State();

  virtual int8_t IO_Init (void);
  virtual void IO_DeInit (void);
  virtual void IO_Delay (uint32_t time);
  virtual int16_t IO_Send (uint8_t *data, uint16_t len, uint32_t timeout);
  virtual int16_t IO_Receive (uint8_t *data, uint16_t len, uint32_t timeout);
};

#endif // SPI_DRV_H

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
