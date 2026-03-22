#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

#ifndef APSSID
#if __has_include("secrets.h")
#include "secrets.h"
#else
#define APSSID "CHANGE_ME"
#define APPSK  "CHANGE_ME_1234"
#endif
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID; //APSSID
const char *password = APPSK; //APPSK

AsyncWebServer server(80);

int speed = 120; //PWM: 0~1023
int speedMotor2 = 180;
int speedMotor4 = 600;

// DC모터 드라이버 연결 핀 
#define m1_dir  2 //gpio2, d9 
#define m1_enb  0 //gpio0, d8
#define m2_dir  13 //gpio13, d7 
#define m2_enb  12 //gpio12, d6 
#define m3_dir  14 //gpio14, d5 
#define m3_enb  4 //gpio4, d4 
#define m4_dir  5 //gpio5, d3 
#define m4_enb  16 //gpio16, d2 

static int val1=2;
static int val2=2;
static int val3=2;
static int val4=2;

void updateMotors();
void m1_goForward(int nSpeed);
void m1_goBackward(int nSpeed);
void m1_stop();
void m2_goForward(int nSpeed);
void m2_goBackward(int nSpeed);
void m2_stop();
void m3_goForward(int nSpeed);
void m3_goBackward(int nSpeed);
void m3_stop();
void m4_goForward(int nSpeed);
void m4_goBackward(int nSpeed);
void m4_stop();

void setup() {
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();

  pinMode(m1_dir, OUTPUT); // m1_dir
  pinMode(m1_enb, OUTPUT); // m1_enb
  pinMode(m2_dir, OUTPUT); // m2_dir
  pinMode(m2_enb, OUTPUT); // m2_enb
  pinMode(m3_dir, OUTPUT); // m3_dir
  pinMode(m3_enb, OUTPUT); // m3_enb
  pinMode(m4_dir, OUTPUT); // m4_dir
  pinMode(m4_enb, OUTPUT); // m4_enb

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
    html += "<title>TEAM10</title>";
    html += "<link rel=\"icon\" href=\"data:,\">";

    html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
    html += "body {width: 100vw;height: 100vh;display: flex;flex-direction: column;justify-content: space-around;align-items: center;}";
    html += ".button-grid {width: 100%;display: flex;flex-direction: row;justify-content: space-around;align-items: center;}";
    html += ".button-control {width: 220px;height: 220px;display: flex;flex-direction: column;align-items: center;}";
    html += ".buttonRow {height: 28%;flex: 1;}";
    html += "button {background-color: #4caf50;width: 20%;height: 90%;text-align: center;line-height: 90%;border: none;color: white;margin: 4px;padding: 10px 30px;}";
    html += ".button-onoff {height: 100%;display: flex;flex-direction: column;justify-content: space-between;align-items: center;}";
    html += ".button-onoff > span {width: 40%;height: 100%;text-align: center;}";
    html += ".motor2,.motor4 {width: 80px;height: 30px;padding: 4px;background-color: #415aff6b;font-weight: bolder;color: #d8d8d8;display: flex;flex-direction: row;justify-content: space-between;align-items: center;}";
    html += ".motor4 {background-color: #ff59009e;}";
    html += ".oN {color: #fff;}";
    html += ".oF {flex: 0.8;height: 100%;display: flex;align-items: center;justify-content: center;}";
    html += ".motor2 > .oN {background-color: #415aff;flex: 0.8;height: 100%;display: flex;align-items: center;justify-content: center;}";
    html += ".motor4 > .oN {background-color: #ff5900;flex: 0.8;height: 100%;display: flex;align-items: center;justify-content: center;}";
    html += "</style>";

    html += "<script>function sendRequest(url) { var xhr = new XMLHttpRequest(); xhr.open('GET', url, true); xhr.send(); }</script>";

    html += "<h4>Team10</h4>";
    html += "<div class='button-grid'>";
    html += "<div class='button-control'>";
    html += "<div class='buttonRow'>";
    html += "<div></div>";
    html += "<button class='button' onclick='sendRequest(\"/forward\")'>F</button>";
    html += "<div></div>";
    html += "</div>";
    html += "<div class='buttonRow'>";
    html += "<button class='button' onclick='sendRequest(\"/left\")'>L</button>";
    html += "<button class='button' onclick='sendRequest(\"/stop\")'>S</button>";
    html += "<button class='button' onclick='sendRequest(\"/right\")'>R</button>";
    html += "</div>";
    html += "<div class='buttonRow'>";
    html += "<div></div>";
    html += "<button class='button' onclick='sendRequest(\"/backward\")'>B</button>";
    html += "<div></div>";
    html += "</div>";
    html += "</div>";
    html += "<div class='button-onoff'>";
    html += "<div class='motor2'>";
    html += "<span class='oN' onclick='sendRequest(\"/motorSecondOn\")'>ON</span>";
    html += "<span class='oF' onclick='sendRequest(\"/motorSecondOFF\")'>OFF</span>";
    html += "</div>";
    html += "<div class='motor4'>";
    html += "<span class='oN' onclick='sendRequest(\"/motorFourthOn\")'>ON</span>";
    html += "<span class='oF' onclick='sendRequest(\"/motorFourthOFF\")'>OFF</span>";
    html += "</div>";
    html += "</div>";
    html += "</div>";
    html += "</body></html>";
    
    request->send(200, "text/html", html);
  });

  server.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request) {
    val1 = 0;
    val3 = 1;
    updateMotors();
    request->send(200, "text/plain", "Forward");
  });

  server.on("/backward", HTTP_GET, [](AsyncWebServerRequest *request) {
    val1 = 1;
    val3 = 0;
    updateMotors();
    request->send(200, "text/plain", "Backward");
  });

  server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request) {
    val1 = 1;
    val3 = 1;
    updateMotors();
    request->send(200, "text/plain", "Left");
  });

  server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request) {
    val1 = 0;
    val3 = 0;
    updateMotors();
    request->send(200, "text/plain", "Right");
  });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request) {
    val1 = 2;
    val3 = 2;
    updateMotors();
    request->send(200, "text/plain", "Stop");
  });
  
  server.on("/motorSecondOn", HTTP_GET, [](AsyncWebServerRequest *request) {
    val2 = 0;
    updateMotors();
    request->send(200, "text/plain", "motorSecondOn");
  });

  server.on("/motorSecondOFF", HTTP_GET, [](AsyncWebServerRequest *request) {
    val2 = 2;
    updateMotors();
    request->send(200, "text/plain", "motorSecondOFF");
  });
  server.on("/motorFourthOn", HTTP_GET, [](AsyncWebServerRequest *request) {
    val4 = 0;
    updateMotors();
    request->send(200, "text/plain", "motorFourthOn");
  });

  server.on("/motorFourthOFF", HTTP_GET, [](AsyncWebServerRequest *request) {
    val4 = 2;
    updateMotors();
    request->send(200, "text/plain", "motorFourthOFF");
  });

  
  
  

  updateMotors();
  server.begin();
}


void loop() {
  //null
}

void updateMotors() {
  if (val1 == 0) {
    m1_goForward(speed);
  } else if (val1 == 1) {
    m1_goBackward(speed);
  } else {
    m1_stop();
  }

  if (val2 == 0) {
    m2_goForward(speedMotor2);
  } else if (val2 == 1) {
    m2_goBackward(speedMotor2);
  } else {
    m2_stop();
  }

  if (val3 == 0) {
    m3_goForward(speed);
  } else if (val3 == 1) {
    m3_goBackward(speed);
  } else {
    m3_stop();
  }

  if (val4 == 0) {
    m4_goForward(speedMotor4);
  } else if (val4 == 1) {
    m4_goBackward(speedMotor4);
  } else {
    m4_stop();
  }
}

void m1_goForward(int nSpeed) {
  digitalWrite(m1_dir, LOW);
  analogWrite(m1_enb, nSpeed);
}

void m1_goBackward(int nSpeed) {
  digitalWrite(m1_dir, HIGH);
  analogWrite(m1_enb, nSpeed);
}

void m1_stop() {
  digitalWrite(m1_dir, LOW);
  analogWrite(m1_enb, 0);
}

void m2_goForward(int nSpeed) {
  digitalWrite(m2_dir, LOW);
  analogWrite(m2_enb, nSpeed);
}

void m2_goBackward(int nSpeed) {
  digitalWrite(m2_dir, HIGH);
  analogWrite(m2_enb, nSpeed);
}

void m2_stop() {
  analogWrite(m2_enb, 0);
}

void m3_goForward(int nSpeed) {
  digitalWrite(m3_dir, LOW);
  analogWrite(m3_enb, nSpeed);
}

void m3_goBackward(int nSpeed) {
  digitalWrite(m3_dir, HIGH);
  analogWrite(m3_enb, nSpeed);
}

void m3_stop() {
  analogWrite(m3_enb, 0);
}

void m4_goForward(int nSpeed) {
  digitalWrite(m4_dir, LOW);
  // GPIO16 does not support PWM on ESP8266, so treat enable as digital.
  digitalWrite(m4_enb, nSpeed > 0 ? HIGH : LOW);
}

void m4_goBackward(int nSpeed) {
  digitalWrite(m4_dir, HIGH);
  digitalWrite(m4_enb, nSpeed > 0 ? HIGH : LOW);
}

void m4_stop() {
  digitalWrite(m4_enb, LOW);
}

