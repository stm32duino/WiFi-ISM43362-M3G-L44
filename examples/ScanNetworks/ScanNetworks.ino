/*

 This example  prints the Wifi device's MAC address, and
 scans for available Wifi networks using the Wifi device.
 Print the SSID, RSSI and Encryption type of each networ in view.
 Every ten seconds, it scans again. It doesn't actually
 connect to any network, so no encryption scheme is specified.

 See es_wifi_conf.h file and ES_WIFI_MAX_DETECTED_AP variable
 for configuring the max of network you can display.
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
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize the WiFi device :
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not present");
    // don't continue:
    while (true);
  }

  // Print firmware version
  String fv = WiFi.firmwareVersion();
  Serial.print("Firwmare version : ");
  Serial.println(fv);
  if (fv != "C3.5.2.3.BETA9")
  {
    Serial.println("Please upgrade the firmware");
  }

  // Print WiFi MAC address:
  printMacAddress();
}

void loop() {
  // scan for existing networks:
  Serial.println("Scanning available networks...");
  listNetworks();
  delay(10000);
}

void printMacAddress() {
  // the MAC address of your Wifi device
  byte mac[6];

  // print your MAC address:
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}

void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.print(" dBm");
    Serial.print("\tEncryption: ");
    printEncryptionType(WiFi.encryptionType(thisNet));
  }
}

void printEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
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
      Serial.println("UNKNOW");
      break;
  }
}
