# WiFi-ISM43362-M3G-L44
Arduino library to support the Wi-Fi module Inventek ISM43362-M3G-L44 (802.11 b/g/n)

## API

The API is based on the Arduino WiFi API. Refer you to the documentations at
https://www.arduino.cc/en/Reference/WiFi

List of the adaptations:

### WiFiST
* Class constructor changed, 3 new constructors depend on the communication driver:
`WiFiClass(SPIClass \*SPIx, uint8_t cs, uint8_t spiIRQ, uint8_t reset, uint8_t wakeup);`
`WiFiClass(HardwareSerial \*UARTx, uint8_t reset, uint8_t wakeup);`
`WiFiClass(uint8_t tx, uint8_t rx, uint8_t reset, uint8_t wakeup);`
* `status()`: less verbose; only `WL_CONNECTED`, `WL_NO_SHIELD`, `WL_IDLE_STATUS`, `WL_NO_SSID_AVAIL`, `WL_SCAN_COMPLETED`, `WL_CONNECT_FAILED` implemented.
* `setMac()`: function added to set MAC address of the Wifi module.

### WiFiClientST :
* `available()`: not supported. Always returns 1.
* `peek()`: not supported. Always returns 0.
* `flush()`: empty function. Do nothing (already empty in Arduino Wifi API).

### WiFiUdpST:
* `endPacket()`:The data are sent when you call `write()`. By default, do nothing and always return 1. If `true` is passed as argument the client connection started by `beginPacket()` is closed.
* `available()`: not supported. Always returns 0.
* `parsePAcket()`: not supported. Always returns 0.
* `peek()`: not supported. Always returns 0.
* `flush()`: Do nothing (already empty in Arduino Wifi API).

### WiFiServerST:
* `status()`: Do nothing (always returns 1).

## Version

> [!IMPORTANT]
> The WiFi library is based on FW "Inventek eS-WiFi ISM43362-M3G-L44-SPI C3.5.2.5.STM".

> [!CAUTION]
>  * WiFiServerST is not stable due to issue of the current WiFi firmware version: C3.5.2.5.STM
>  * WEP-128 is not functional. Issue probably due to the current WiFi firmware version: C3.5.2.5.STM
>  * UDP server is not functional with the current WiFi firmware version: C3.5.2.5.STM while it was with FW version C3.5.2.3.BETA9 (#12)

To update the Inventek ISM-43362 Wi-Fi module firmware, please read the readme file for instructions
include in this archive:
https://www.st.com/resource/en/utilities/inventek_fw_updater.zip

## Supported Secure Wi-Fi authentication by the Wi-Fi module Inventek ISM43362-M3G-L44:
* WEP-128
* WPA-PSK (TKIP)
* WPA2-PSK

## Examples

The examples are close of the Arduino WiFi library but with some adaptations to work
with the ISM43362-M3G-L44 WiFi module.

## Documentation

You can find the source files at  
https://github.com/stm32duino/WiFi-ISM43362-M3G-L44

The ISM43362-M3G-L44 datasheet is available at  
https://www.inventeksys.com/ism4336-m3g-l44-e-embedded-serial-to-wifi-module/
