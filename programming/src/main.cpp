#include <Arduino.h>
#include <GyverStepper.h>
#include "GyverPlanner.h"

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "SPIFFS.h"

#include "USB.h"
USBCDC USBSerial;

GStepper<STEPPER2WIRE> turntable(14400, 8, 13);  // X
GStepper<STEPPER2WIRE> axis1(14400, 6, 14);      // Y
GStepper<STEPPER2WIRE> axis2(14400, 4, 10);      // Z
GStepper<STEPPER2WIRE> axis3(12288, 11, 12);

GPlanner<STEPPER2WIRE, 4> planner;

const int conversion[] = {14400/360, 14400/360, 14400/360, 12288/360};

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int pos[4];

bool setFKposition(float a0, float a1, float a2, float a3) {
    int pos[] = {int(a0), int(a1), int(a2), int(a3)};
    planner.setTarget(pos);
    return true;
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    String dataString = "";
    
    uint8_t iter = 0;
  switch (type) {
    case WS_EVT_CONNECT:
        USBSerial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        USBSerial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        USBSerial.println(len);

        
        
        for(int i =0; i< len; i++) {
            Serial.printf("%d, %d = %c\n", i, data[i], data[i]);
            if(char(data[i]) != ',') dataString+=char(data[i]);
            else {
                pos[iter] = dataString.toInt();
                iter+=1;
                dataString="";
            }
        }

        digitalWrite(1, dataString.toInt()==1);

        for (int i: pos) {
            USBSerial.printf("%d,", i);
        }

        for (int i = 0; i < 4; i++) {
            pos[i] *= conversion[i];
        }

        planner.setTarget(pos);
        //if(len == 4) setFKposition(data[0], data[1], data[2], data[3]);

        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
  }
}

void setup() {
    USBSerial.begin();  // Скорость указывать не нужно!
    USB.begin();

    if(!SPIFFS.begin(true)){
        USBSerial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    WiFi.begin("spynet-2.4g", "MW9pDbkK");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        USBSerial.println("Connecting to WiFi..");
    }

    USBSerial.println(WiFi.localIP());

    while(digitalRead(0) != 0) delay(100);

    pinMode(9, OUTPUT);
    digitalWrite(9, LOW);  // ENABLE W

    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);  // ENABLE XYZ

    pinMode(1, OUTPUT);

    turntable.setRunMode(FOLLOW_POS);
    axis1.setRunMode(FOLLOW_POS);
    axis2.setRunMode(FOLLOW_POS);
    axis3.setRunMode(FOLLOW_POS);

    axis1.reverse(true);
    axis3.reverse(true);

    planner.addStepper(0, turntable);
    planner.addStepper(1, axis1);
    planner.addStepper(2, axis2);
    planner.addStepper(3, axis3);

    planner.setAcceleration(300);
    planner.setMaxSpeed(800);

    int homingPos[] = {0, (33+90)*(14400/360), (180-19)*(-14400/360), 134*conversion[3]};

    planner.setTarget(homingPos);

    while(!planner.ready()) planner.tick();

    planner.reset();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false);
    });

    server.serveStatic("/", SPIFFS, "/"); 

    ws.onEvent(onEvent);
    server.addHandler(&ws);

    server.begin();

    // axis1.setTarget(1000);
}

void loop() {
    ws.cleanupClients();
    planner.tick();
}