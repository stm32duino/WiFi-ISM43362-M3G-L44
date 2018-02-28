/*
 * WiFiUDPSendReceiveString
 *
 * This sketch uses the WiFi module to initialize an UDP server.
 * that listens for UDP packets on a port named localPort.
 * When a packet is received, an Acknowledge packet is sent to the client on port remotePort
 * It also prints the packet content, the IP address and port of remote client.
 *
 */

#include <SPI.h>
#include <WiFiST.h>
#include <WiFiUdpST.h>

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
int status = WL_IDLE_STATUS;
char ssid[] = "yourNetwork";             // your network SSID (name)
char pass[] = "secretPassword";          // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                        // your network key Index number (needed only for WEP)

unsigned int localPort = 3000;           // local port to listen on

char packetBuffer[255];                  // buffer to hold incoming packet
char replyBuffer[] = "acknowledged";     // a string to send back

WiFiUDP Udp;

void setup() {
  // Initialize serial communication:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize the WiFi device:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not detected");
    // don't continue:
    while (true);
  }

  // Print firmware version:
  String fv = WiFi.firmwareVersion();
  Serial.print("Firwmare version: ");
  Serial.println(fv);

  if (fv != "C3.5.2.3.BETA9") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WiFi network ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected.\nNetwork information:");
  printWifiStatus();

  Serial.println("\nStarting UDP server...");
  // if you get a connection, report back via serial:
  Udp.begin(localPort);
}

void loop() {

  // read the packet into packetBufffer
  int len = Udp.read(packetBuffer, 255);
  if (len > 0) {
    Serial.print("Received a new packet from ");
    IPAddress remoteIp = Udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    Serial.println("Contents:");
    Serial.println(packetBuffer);

    // send a reply, to the client that sent us the packet we received
    Udp.write(replyBuffer);
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the IP address of your WiFi module:
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

  // print the received signal strength:
  int32_t rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}
