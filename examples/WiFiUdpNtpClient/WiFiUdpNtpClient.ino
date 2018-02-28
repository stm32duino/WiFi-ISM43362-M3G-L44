/*
 * WiFiUdpNtpClient
 *
 * Get the time from a Network Time Protocol (NTP) time server.
 * For more on NTP time servers and the messages needed to communicate with them,
 * see http://en.wikipedia.org/wiki/Network_Time_Protocol
 *
 * Print the time in second since 1 january 1900.
 * Print Unix time and UTC time.
 *
 * This code is in the public domain.
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
char ssid[] = "yourNetwork";          // your network SSID (name)
char pass[] = "secretPassword";       // your network password
int keyIndex = 0;                     // your network key Index number (needed only for WEP)

unsigned int localPort = 2390;        // local port to listen for UDP packets

IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server IP

const int NTP_PACKET_SIZE = 48;       // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE];  // buffer to hold incoming and outgoing packets

bool ConnectionCreate = false;

// An UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

void setup() {
  // Open serial communication:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  // initialize the WiFi module:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not detcted");
    // don't continue:
    while (true);
  }

  // print the firmware version:
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

  Serial.println("Connected.\nWiFi network status:");
  printWifiStatus();

  Serial.println("\nStarting connection to NTP server...");
}

void loop() {
  // send an NTP packet to a time server
  sendNTPpacket(timeServer);
  // wait to see if a reply is available
  delay(1000);
  
  // read the packet and store it into the buffer
  int len = Udp.read(packetBuffer, NTP_PACKET_SIZE);
  if (len > 0) {
    Serial.println("New NTP packet received");

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer:
    // this is the NTP time (seconds since Jan 1 1900)
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900: ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time: ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);

    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ((epoch % 60) < 10) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
  }
  
  // wait ten seconds before asking for the time again
  delay(10000);
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress& address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  // Initialize values needed to form NTP request
  // (see URL above for details on the NTP packet format)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;            // Stratum, or type of clock
  packetBuffer[2] = 6;            // Polling Interval
  packetBuffer[3] = 0xEC;         // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  if (!ConnectionCreate) {
    // NTP requests are sent to port 123
    Udp.beginPacket(address, 123);
    ConnectionCreate = true;
  }
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
}

void printWifiStatus() {
  // print the SSID of the network you're connected to:
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

  // print the received signal strength (RSSI):
  int32_t rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}
