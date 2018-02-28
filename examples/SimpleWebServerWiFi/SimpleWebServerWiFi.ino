/*
 * SimpleWebServerWiFi
 *
 * A simple web server that lets you blink an LED via a Web Browser.
 * This sketch will print the IP address of your WiFi module to the Serial Monitor
 * You can use that IP address with a web browser to turn on and off the LED on pin 9.
 *
 * If the IP address of your WiFi module is, for example, yourIPAddress:
 * http://yourIPAddress/H turns the LED on
 * http://yourIPAddress/L turns the LED off
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
  // Initialize serial and wait for serial port to connect:
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  // set the LED pin mode
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize the WiFi module:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi module not detected");
    // don't continue
    while (true);
  }

  // print firmware version:
  String fv = WiFi.firmwareVersion();
  Serial.print("Firwmare version: ");
  Serial.println(fv);

  if (fv != "C3.5.2.3.BETA9") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WiFi network ");
    // print the network name (SSID)
    Serial.println(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected.\nNetwork information:");
  // you're connected now, so print out the status
  printWifiStatus();
  // start the web server on port 80
  server.begin();
}

void loop() {

  client = server.available();               // listen for incoming clients

  if (client) {
    char c = 0x35;                           // if you get a client,
    Serial.println("New client connected");  // print a message out the serial port
    String currentLine = "";                 // make a String to hold incoming data from the client
    while (client.connected()) {             // loop while the client's connected
      dataRead(&c);
      if (c == '\n') {                       // if the byte is a newline character
        // if the current line is blank, you got two newline characters in a row.
        // that's the end of the client HTTP request, so send a response:
        if (currentLine.length() == 0) {
          // HTTP header, separe the body with a blank line
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Length: 222");
          client.println("Connection: Close");
          client.println("Content-Type: text/html; charset=iso-8859-1");
          client.println();

          // the content of the HTTP response follows the header:
          client.print("<html><head>\n<title>WiFi Web Server</title>\n</head>");
          client.print("<body>\n<h1>ST Web Server</h1>\n");
          client.print("Click <a href=\"/H\">here</a> turn the LED on LED_BUILTIN on<br>\n");
          client.print("Click <a href=\"/L\">here</a> turn the LED on LED_BUILTIN off<br>");

          // The HTTP response ends with another blank line:
          client.print("</body></html>\n");

          // break out of the while loop:
          break;

        } else {  // if you got a newline, then clear currentLine:
          currentLine = "";
        }

      } else if (c != '\r') {   // if you got anything else but a carriage return character,
        currentLine += c;       // add it to the end of the currentLine
      }

      // Check to see if the client request was "GET /H" or "GET /L":
      if (currentLine.endsWith("GET /H")) {
        digitalWrite(LED_BUILTIN, HIGH);        // GET /H turns the LED on
      }

      if (currentLine.endsWith("GET /L")) {
        digitalWrite(LED_BUILTIN, LOW);         // GET /L turns the LED off
      }
    }
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
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

/*
 * Read data in the WiFi device. Reading byte by byte in very slow. This function
 * allows to read READ_BUFFER byte in the device and returns byte by byte the result.
 */
int dataRead(char *c) {
  if (read_index == READ_BUFFER) {     // No data available in the current buffer
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
