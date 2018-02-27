/*
 * ScanNetworks
 *
 * This example uses the ISM43362-M3G-L44 WiFi module to scan for available
 * WiFi networks every ten seconds and prints their SSID, RSSI and encryption type.
 * It also prints the MAC Address of the WiFi module.
 * It doesn't actually connect to any network, so no encryption scheme is specified.
 *
 * See es_wifi_conf.h file and ES_WIFI_MAX_DETECTED_AP variable
 * for configuring the max of network you can display.
 */

#include <SPI.h>
#include <WiFiST.h>

/*
  The following configuration is dedicated to the DISCO L475VG IoT board.
  You should adapt it to your board.

Configure SPI3:
 * MOSI: PC12
 * MISO: PC11
 * SCLK: PC10

Configure WiFi:
 * SPI         : SPI3
 * Cs          : PE0
 * Data_Ready  : PE1
 * reset       : PE8
 * wakeup      : PB13
 */

SPIClass SPI_3(PC12, PC11, PC10);
WiFiClass WiFi(&SPI_3, PE0, PE1, PE8, PB13);

void setup() {
  // Initialize serial communication and wait for serial port to connect:
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // initialize the WiFi module:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not detected");
    // don't continue:
    while (true);
  }

  // print firmware version:
  String fv = WiFi.firmwareVersion();
  Serial.print("Firmware version: ");
  Serial.println(fv);
  if (fv != "C3.5.2.3.BETA9") {
    Serial.println("Please upgrade the firmware");
  }

  // Print WiFi MAC address:
  printMacAddress();
}

void loop() {
  // scan for existing networks every 10 seconds
  Serial.println("Scanning available networks...");
  listNetworks();
  delay(10000);
}

void printMacAddress() {
  // print the MAC address of your Wifi module:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  for (uint8_t i = 0; i < 6; i++) {
    if (mac[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i != 5) {
      Serial.print(":");
    } else {
      Serial.println();
    }
  }
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("No WiFi network available");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("Number of available networks: ");
  Serial.println(numSsid);

  // print the network number, name, RSSI and encryption type for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(")\tSSID: ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tRSSI: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
  Serial.println();
}

void printEncryptionType(uint8_t encryptionType) {
  // read the encryption type and print out the name:
  switch (encryptionType) {
    case ES_WIFI_SEC_OPEN:
      Serial.println("OPEN");
      break;
    case ES_WIFI_SEC_WEP:
      Serial.println("WEP");
      break;
    case ES_WIFI_SEC_WPA:
      Serial.println("WPA");
      break;
    case ES_WIFI_SEC_WPA2:
      Serial.println("WPA2");
      break;
    case ES_WIFI_SEC_WPA_WPA2:
      Serial.println("WPA_WPA2");
      break;
    case ES_WIFI_SEC_WPA2_TKIP:
      Serial.println("WPA_TKIP");
      break;
    case ES_WIFI_SEC_UNKNOWN:
    default:
      Serial.println("UNKNOWN");
      break;
  }
}
