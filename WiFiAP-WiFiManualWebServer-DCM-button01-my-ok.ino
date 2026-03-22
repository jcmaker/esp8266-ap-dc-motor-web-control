#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

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

//ESP8266WebServer server(80);
WiFiServer server(80);

int speed = 550; //PWM: 0~1023

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

void setup() {
  Serial.begin(115200);
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.begin();
  //Serial.println("HTTP server started");

  //======================
  pinMode(m1_dir, OUTPUT); // m1_dir
  pinMode(m1_enb, OUTPUT); // m1_enb
  pinMode(m2_dir, OUTPUT); // m2_dir
  pinMode(m2_enb, OUTPUT); // m2_enb
  pinMode(m3_dir, OUTPUT); // m3_dir
  pinMode(m3_enb, OUTPUT); // m3_enb
  pinMode(m4_dir, OUTPUT); // m4_dir
  pinMode(m4_enb, OUTPUT); // m4_enb
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  //Serial.println(F("new client"));

  client.setTimeout(2000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  //Serial.println(F("request: "));
  //Serial.println(req);

  // Match the request
  //int val;
  if (req.indexOf(F("/m1/f")) != -1) {
    val1 = 0;
  } else if (req.indexOf(F("/m1/b")) != -1) {
    val1 = 1;
  } else if (req.indexOf(F("/m1/s")) != -1) {
    val1 = 2;
  } else if (req.indexOf(F("/m2/f")) != -1) {
    val2 = 0;
  } else if (req.indexOf(F("/m2/b")) != -1) {
    val2 = 1;
  } else if (req.indexOf(F("/m2/s")) != -1) {
    val2 = 2;
  } else if (req.indexOf(F("/m3/f")) != -1) {
    val3 = 0;
  } else if (req.indexOf(F("/m3/b")) != -1) {
    val3 = 1;
  } else if (req.indexOf(F("/m3/s")) != -1) {
    val3 = 2;
  } else if (req.indexOf(F("/m4/f")) != -1) {
    val4 = 0;
  } else if (req.indexOf(F("/m4/b")) != -1) {
    val4 = 1;
  } else if (req.indexOf(F("/m4/s")) != -1) {
    val4 = 2;
  } else {
    Serial.println(F("invalid request"));
  }
  /*
  Serial.print("val1:");
  Serial.println(val1);
  Serial.print("val2:");
  Serial.println(val2);
  Serial.print("val3:");
  Serial.println(val3);
  Serial.print("val4:");
  Serial.println(val4);
  */

  if(val1 == 0){
    m1_goForward(speed);
  }
  else if(val1 == 1){
    m1_goBackward(speed);
  }
  else{
    m1_stop();
  }

  if(val2 == 0){
    m2_goForward(speed);
  }
  else if(val2 == 1){
    m2_goBackward(speed);
  }
  else{
    m2_stop();
  }

  if(val3 == 0){
    m3_goForward(speed);
  }
  else if(val3 == 1){
    m3_goBackward(speed);
  }
  else{
    m3_stop();
  }

  if(val4 == 0){
    m4_goForward(speed);
  }
  else if(val4 == 1){
    m4_goBackward(speed);
  }
  else{
    m4_stop();
  }

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  //client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now "));
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  // Display the HTML web page
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<title>DC Motor Control</title>");
  client.println("<link rel=\"icon\" href=\"data:,\">");
  // CSS to style the on/off buttons 
  // Feel free to change the background-color and font-size attributes to fit your preferences
  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 5px 30px;");
  client.println("text-decoration: none; font-size: 15px; margin: 2px; cursor: pointer;}");
  client.println(".button1 {background-color: #cc603f;}");
  client.println(".button2 {background-color: #3fcc44;}");
  client.println(".button3 {background-color: #2e2eef;}");
  client.println(".button4 {background-color: #f40b0b;}"); 
  client.println(".button5 {background-color: #555555;}"); 
  client.println(".button6 {background-color: #000000;}</style></head>");
  // Web Page Heading
  client.println("<body><h1>Mechanical Design</h1>");
  
  //client.print(F("<P><a href='http://"));
  client.print(F("<P><a href='"));
  //client.print("192.168.4.1");
  //client.print(F("/m1/f'><button class=\"button\">M1-F</button></a> <a href='http://"));
  //client.print(F("/m1/f'><button class=\"button\">M1-F</button></a> <a href='"));
  client.print(F("/m1/f'><button class=\"button button1\">M1-F</button></a> <a href='"));
  //client.print("192.168.4.1");
  //client.print(F("/m1/b'><button class=\"button\">M1-B</button></a> <a href='http://"));
  client.print(F("/m1/b'><button class=\"button button1\">M1-B</button></a> <a href='"));
  //client.print("192.168.4.1");
  client.print(F("/m1/s'><button class=\"button button6\">M1-S</button></a> "));
  if(val1 == 0){
    client.print(F("m1_f</P>"));
  }
  else if(val1 == 1){
    client.print(F("m1_b</P>"));
  }
  else{
    client.print(F("m1_s</P>"));
  }

  //client.print(F("<P><a href='http://"));
  client.print(F("<P><a href='"));
  //client.print("192.168.4.1");
  //client.print(F("/m2/f'><button class=\"button\">M2-F</button></a> <a href='http://"));
  client.print(F("/m2/f'><button class=\"button button2\">M2-F</button></a> <a href='"));
  //client.print("192.168.4.1");
  //client.print(F("/m2/b'><button class=\"button\">M2-B</button></a> <a href='http://"));
  client.print(F("/m2/b'><button class=\"button button2\">M2-B</button></a> <a href='"));
  //client.print("192.168.4.1");
  client.print(F("/m2/s'><button class=\"button button6\">M2-S</button></a> "));
  if(val2 == 0){
    client.print(F("m2_f</P>"));
  }
  else if(val2 == 1){
    client.print(F("m2_b</P>"));
  }
  else{
    client.print(F("m2_s</P>"));
  }

  //client.print(F("<P><a href='http://"));
  client.print(F("<P><a href='"));
  //client.print("192.168.4.1");
  //client.print(F("/m3/f'><button class=\"button\">M3-F</button></a> <a href='http://"));
  client.print(F("/m3/f'><button class=\"button button3\">M3-F</button></a> <a href='"));
  //client.print("192.168.4.1");
  //client.print(F("/m3/b'><button class=\"button\">M3-B</button></a> <a href='http://"));
  client.print(F("/m3/b'><button class=\"button button3\">M3-B</button></a> <a href='"));
  //client.print("192.168.4.1");
  client.print(F("/m3/s'><button class=\"button button6\">M3-S</button></a> "));
  if(val3 == 0){
    client.print(F("m3_f</P>"));
  }
  else if(val3 == 1){
    client.print(F("m3_b</P>"));
  }
  else{
    client.print(F("m3_s</P>"));
  }

  //client.print(F("<P><a href='http://"));
  client.print(F("<P><a href='"));
  //client.print("192.168.4.1");
  //client.print(F("/m4/f'><button class=\"button\">M4-F</button></a> <a href='http://"));
  client.print(F("/m4/f'><button class=\"button button4\">M4-F</button></a> <a href='"));
  //client.print("192.168.4.1");
  //client.print(F("/m4/b'><button class=\"button\">M4-B</button></a> <a href='http://"));
  client.print(F("/m4/b'><button class=\"button button4\">M4-B</button></a> <a href='"));
  //client.print("192.168.4.1");
  client.print(F("/m4/s'><button class=\"button button6\">M4-S</button></a> "));
  if(val4 == 0){
    client.print(F("m4_f</P>"));
  }
  else if(val4 == 1){
    client.print(F("m4_b</P>"));
  }
  else{
    client.print(F("m4_s</P>"));
  }

  client.print(F("<meta http-equiv=\"refresh\" content=\"5\">"));
  client.print(F("</body></html>"));
  
  client.stop();  
}

void m1_goForward(int nSpeed)
{
    digitalWrite(m1_dir, LOW);
    analogWrite(m1_enb, nSpeed);
}

void m1_goBackward(int nSpeed)
{
    digitalWrite(m1_dir, HIGH);
    analogWrite(m1_enb, nSpeed);    
}

void m1_stop(void)
{
    analogWrite(m1_enb, 0);
}

void m2_goForward(int nSpeed)
{
    digitalWrite(m2_dir, LOW);
    analogWrite(m2_enb, nSpeed);
}

void m2_goBackward(int nSpeed)
{
    digitalWrite(m2_dir, HIGH);
    analogWrite(m2_enb, nSpeed);
}

void m2_stop(void)
{
    analogWrite(m2_enb, 0);
}

void m3_goForward(int nSpeed)
{
    digitalWrite(m3_dir, LOW);
    analogWrite(m3_enb, nSpeed);
}

void m3_goBackward(int nSpeed)
{
    digitalWrite(m3_dir, HIGH);
    analogWrite(m3_enb, nSpeed);
}

void m3_stop(void)
{
    analogWrite(m3_enb, 0);
}

void m4_goForward(int nSpeed)
{
    digitalWrite(m4_dir, LOW);
    analogWrite(m4_enb, nSpeed);
}

void m4_goBackward(int nSpeed)
{
    digitalWrite(m4_dir, HIGH);
    analogWrite(m4_enb, nSpeed);
}

void m4_stop(void)
{
    analogWrite(m4_enb, 0);
}
