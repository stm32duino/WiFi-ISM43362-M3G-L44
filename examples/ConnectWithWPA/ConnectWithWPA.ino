/*
 * ConnectWithWPA
 *
 * This example is used to connect to an WPA2-encrypted WiFi network.
 * Then it prints the MAC address of the WiFi module,
 * the IP address obtained through DHCP, and other network information.
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

char ssid[] = "ST_test";         // your network SSID (name)
char pass[] = "secretPassword";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

void setup() {
  // initialize serial communication and wait for serial monitor to open:
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

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA/WPA2 network: ");
    Serial.println(ssid);
    // Connect to WPA2 network:
    status = WiFi.begin(ssid, pass);
    if (status != WL_CONNECTED) {
      // Connect to WPA (TKIP) network:
      status = WiFi.begin(ssid, pass, ES_WIFI_SEC_WPA);
    }

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the connection info:
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
