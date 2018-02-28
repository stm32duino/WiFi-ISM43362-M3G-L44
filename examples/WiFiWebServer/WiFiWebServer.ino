/*
 * WiFiWebServer
 *
 * A simple web server that shows the value of the analog input pins
 * using the WiFi module ISM43362-M3G-L44.
 *
 * This example is written for a network using WPA encryption.
 * For WEP or WPA, change the Wifi.begin() call accordingly.
 *
 */

#include <SPI.h>
#include <WiFiST.h>

#include <WiFiServerST.h>
#include <WiFiClientST.h>

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

const int READ_BUFFER = 100;             // Size of reading buffer in WiFi device
uint8_t buf_read[READ_BUFFER]= {0};      // Reading buffer
int8_t read_index = READ_BUFFER;         // Reading index in buf_read

WiFiClient client;

char ssid[] = "yourNetwork";      // your network SSID (name)
char pass[] = "secretPassword";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  // Initialize serial communication:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize the WiFi module:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not detcted");
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

  // you're connected now, so print out the status
  Serial.println("Connected.\nNetwork information:");
  printWifiStatus();
  server.begin();      // start the web server on port 80
}


void loop() {

  client = server.available();       // listen for incoming clients

  if (client) {  // if a new client is connetcted,
    char c = 0x35;
    Serial.println("New client connected");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      dataRead(&c);
      // if you've received a newline character and the line is blank
      // the http request has ended, so you can send a reply
      if (c == '\n' && currentLineIsBlank) {
        // send a standard http response header
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");  // the connection will be closed after completion of the response
        client.println("Refresh: 10");        // refresh the page automatically every 10 seconds
        client.println();
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        // output the value of each analog input pin
        for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
          int sensorReading = analogRead(analogChannel);
          client.print("analog input ");
          client.print(analogChannel);
          client.print(" is ");
          client.print(sensorReading);
          client.println("<br />");
        }
        client.println("</html>");
        break;
      }

      if (c == '\n') {
        // you're starting a new line
        currentLineIsBlank = true;
      } else if (c != '\r') {
        // you've received a character on the current line
        currentLineIsBlank = false;
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    server.begin();
    Serial.println("Client disconnected");
  }
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
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

/*
 * Read data in the WiFi device. Reading byte by byte in very slow, so this function allows to
 * read READ_BUFFER byte in the device and return byte by byte the result.
 */
int dataRead(char *c) {
  if (read_index == READ_BUFFER) {        // No data available in the current buffer
    client.read(buf_read, READ_BUFFER);
    read_index = 0;
  }
  *c = buf_read[read_index];
  read_index++;

  if (*c != '\0') {
    return 1;
  } else {
    return 0;
  }
}
