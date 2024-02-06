// #include <WiFi.h>
// #include <WiFiClient.h>
// #include <WebServer.h>
// #include <ESPmDNS.h>

// #include <GyverStepper.h>

// #include "USB.h"
// USBCDC USBSerial;

// const char* ssid = "spynet-2.4g";
// const char* password = "MW9pDbkK";

// WebServer server(80);

// const int led = 13;

// void handleRoot() {
//   digitalWrite(led, 1);
//   server.send(200, "text/plain", "hello from esp32!");
//   digitalWrite(led, 0);
// }

// void handleNotFound() {
//   digitalWrite(led, 1);
//   String message = "File Not Found\n\n";
//   message += "URI: ";
//   message += server.uri();
//   message += "\nMethod: ";
//   message += (server.method() == HTTP_GET) ? "GET" : "POST";
//   message += "\nArguments: ";
//   message += server.args();
//   message += "\n";
//   for (uint8_t i = 0; i < server.args(); i++) {
//     message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
//   }
//   server.send(404, "text/plain", message);
//   digitalWrite(led, 0);
// }

// void setup(void) {
//   USBSerial.begin();  // Скорость указывать не нужно!
//   USB.begin();
//   pinMode(led, OUTPUT);
//   digitalWrite(led, 0);
  
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);
//   USBSerial.println("");

//   // Wait for connection
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   USBSerial.println("");
//   USBSerial.print("Connected to ");
//   USBSerial.println(ssid);
//   USBSerial.print("IP address: ");
//   USBSerial.println(WiFi.localIP());

//   if (MDNS.begin("esp32")) {
//     USBSerial.println("MDNS responder started");
//   }

//   server.on("/", handleRoot);

//   server.on("/inline", []() {
//     server.send(200, "text/plain", "this works as well");
//   });

//   server.onNotFound(handleNotFound);

//   server.begin();
//   USBSerial.println("HTTP server started");
// }

// void loop(void) {
//   server.handleClient();
//   delay(2);//allow the cpu to switch to other tasks
// }