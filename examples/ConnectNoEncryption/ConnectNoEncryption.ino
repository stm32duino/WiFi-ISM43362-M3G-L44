/*
 * ConnectNoEncryption
 *
 * This example is used to connect to an unencrypted WiFi network.
 * Then it prints the MAC address of the WiFi module,
 * the IP address obtained, and other network information.
 * It also shows how to set the MAC address of the WiFi module.
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

char ssid[] = "ST_test";                      // the name of your network
int status = WL_IDLE_STATUS;                  // the Wifi radio's status
uint8_t macAddrDevice[6] = {0,1,2,3,4,5};     // the mac address you want set in the WiFi device

void setup() {
  // initialize serial communication and wait for serial port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // initialize the wifi module:
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

  // Set Mac Address of the device on the board (optional)
  // WiFi.setMac(macAddrDevice);
  // Be careful, if setting the MAC adress, you could lose
  // previously assigned MAC address. If only for test,
  // you can save your current MAC address, change it,
  // then set it back to original one.

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to the open WiFi network ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the info about the network:
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

  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(subnet);

  // print your gateway IP address:
  IPAddress gatewayIP = WiFi.gatewayIP();
  Serial.print("Gateway IP: ");
  Serial.println(gatewayIP);
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the BSSID which is the MAC address of the router you're attached to:
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
