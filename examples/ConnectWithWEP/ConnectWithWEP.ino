/*
 * ConnectWithWEP 
 *
 * This example is used to connect to a WEP-encrypted WiFi network.
 * Then it prints the MAC address of the WiFi module,
 * the IP address obtained, and other network details.
 *
 * If you use 40-bit WEP, you need a key that is 10 characters long,
 * and the characters must be hexadecimal (0-9 or A-F).
 * e.g.  for 40-bit, ABBADEAF01 will work, but ABBADEAF won't work
 * (too short) and ABBAISDEAF won't work (I and S are not
 * hexadecimal characters).
 *
 * For 128-bit, you need a string that is 26 characters long.
 * D0D0DEADF00DABBADEAFBEADED will work because it's 26 characters,
 * all in the 0-9, A-F range.
 *
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

char ssid[] = "ST_test";                         // your network SSID (name)
char key[] = "THISISTHETESTKEYFORSTDUINO";       // your network key
int keyIndex = 0;                                // your network key Index number
int status = WL_IDLE_STATUS;                     // the Wifi radio's status

void setup() {
  // initialize serial communication and wait for serial port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // initialize the WIFI device:
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

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WEP network ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, keyIndex, key);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // once you are connected print out the network status
  Serial.println("Connected.\nNetwork information:");
  printCurrentNet();
  printWifiData();
}

void loop() {
  // check the network connection once every 10 seconds:
  delay(10000);
  Serial.println("*** Network information ***");
  printCurrentNet();
}

void printWifiData() {
  // print your WiFi device's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the MAC address of your WiFi module:
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

void printCurrentNet() {
  // print the SSID of the network you're connected to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're connected to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  for (uint8_t i = 0; i < 6; i++) {
    if (bssid[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(bssid[i], HEX);
    if (i != 5) {
      Serial.print(":");
    } else {
      Serial.println();
    }
  }

  // print the received signal strength:
  int32_t rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.println(rssi);

  // print the encryption type:
  Serial.print("Encryption Type: ");
  printEncryptionType(WiFi.encryptionType());
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
