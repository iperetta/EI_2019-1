#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* Put your SSID & Password */
const char* ssid = "REDE-WIFI-PARA-CONEXAO";  // Enter SSID here
const char* password = "SENHA-DA-REDE-WIFI";  //Enter Password here

ESP8266WebServer server(80);

uint8_t pinR = D2;
uint8_t pinG = D5;
uint8_t pinB = D6;
bool LEDstatus = LOW;
int valR, valG, valB;

void setup() {   
  Serial.begin(9600); 
  
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  server.on("/", handle_OnConnect);
  server.on("/ledon", handle_ledON);
  server.on("/ledoff", handle_ledOFF);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
  if(LEDstatus) {
    analogWrite(pinR, map(valR, 0, 255, 0, 1023));
    analogWrite(pinG, map(valG, 0, 255, 0, 1023));
    analogWrite(pinB, map(valB, 0, 255, 0, 1023));
  }
  else {
    analogWrite(pinR, 0);
    analogWrite(pinG, 0);
    analogWrite(pinB, 0);
  }
}

void handle_OnConnect() {
  LEDstatus = LOW;
  Serial.println("LED Status: OFF");
  server.send(200, "text/html", SendHTML(LEDstatus)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

void handle_ledON() {
  static int i = 0;
  LEDstatus = HIGH;
  Serial.println("LED Status: ON");
  server.send(200, "text/html", SendHTML(HIGH)); 
  valR = (server.hasArg("r"))? server.arg("r").toInt() : 0xFF;
  valG = (server.hasArg("g"))? server.arg("g").toInt() : 0xFF;
  valB = (server.hasArg("b"))? server.arg("b").toInt() : 0xFF;
}

void handle_ledOFF() {
  LEDstatus = LOW;
  Serial.println("LED Status: OFF");
  server.send(200, "text/html", SendHTML(LOW)); 
}

String SendHTML(bool ledstat) {
  String ptr = "<!DOCTYPE html> <html>";

  ptr += "<head>";
  ptr +="<meta charset=\"utf-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  ptr += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css\">";
  ptr += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/js/bootstrap.min.js\"></script>";
  ptr += "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>";

  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}";
  ptr +=".button-on {background-color: #1abc9c;}";
  ptr +=".button-on:active {background-color: #16a085;}";
  ptr +=".button-off {background-color: #34495e;}";
  ptr +=".button-off:active {background-color: #2c3e50;}";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}";
  ptr +="</style>";
  
  ptr +="</head>";
  ptr += "<body><div class=\"container\"><div class=\"row\"><h1>ESP Color Picker</h1>";

  if(ledstat) {
    ptr +="<p>LED Status: <a class=\"button button-on\" href=\"/ledoff\">ON</a></p>";
    ptr += "<a type=\"submit\" id=\"change_color\" type=\"button\" class=\"btn btn-primary\">Change Color</a>";
    ptr += "<input class=\"jscolor {onFineChange:'update(this)'}\" id=\"rgb\">";
  }
  else {
    ptr +="<p>LED Status: <a class=\"button button-off\" href=\"/ledon\">OFF</a></p>";
  }
  
  ptr += "</div></div>";
  ptr += "<script>";
  ptr += "function update(picker) {document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' +  Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);";
  ptr += "document.getElementById(\"change_color\").href=\"?r=\" + Math.round(picker.rgb[0]*4.0117) + \"&g=\" +  Math.round(picker.rgb[1]*4.0117) + \"&b=\" + Math.round(picker.rgb[2]*4.0117);}";
  ptr += "</script>";
  ptr += "</body></html>";

  return ptr;
}
