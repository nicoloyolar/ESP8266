
// se declaran las entradas al módulo provenientes del driver
int pin1 = D1;
int pin2 = D2;  
int pin3 = D3;
int pin4 = D4; 

// se definen los polos del motor
int pole1[] ={0,0,0,0, 0,1,1,1, 0};
int pole2[] ={0,0,0,1, 1,1,0,0, 0};
int pole3[] ={0,1,1,1, 0,0,0,0, 0};
int pole4[] ={1,1,0,0, 0,0,0,1, 0};

int poleStep = 0; 
int  dirStatus = 3;

// botones para activación y sentido de giro del motor
String buttonTitle1[] ={"Forward", "Reverse"};
String buttonTitle2[] ={"Forward", "Reverse"};
String argId[] ={"Forward", "Reverse"};

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// se definen los parámetros para conectarse a la red... se deben cambiar por la red local
#ifndef STASSID
#define STASSID "Depto 804B"
#define STAPSK  "19083214"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

// se define el servidor web que escucha HTTP en el puerto 80
ESP8266WebServer server(80);

void handleRoot() {

 String HTML ="<!DOCTYPE html>\
  <html>\
  <head>\
  \t\n<title>Control Baño Antimanchas</title>\
  \t\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  \n<style>\
 \nhtml,body{\t\nwidth:100%\;\nheight:100%\;\nmargin:0}\n*{box-sizing:border-box}\n.colorAll{\n\tbackground-color:#90ee90}\n.colorBtn{\n\tbackground-color:#add8e6}\n.angleButtdon,a{\n\tfont-size:30px\;\nborder:1px solid #ccc\;\ndisplay:table-caption\;\npadding:7px 10px\;\ntext-decoration:none\;\ncursor:pointer\;\npadding:5px 6px 7px 10px}a{\n\tdisplay:block}\n.btn{\n\tmargin:5px\;\nborder:none\;\ndisplay:inline-block\;\nvertical-align:middle\;\ntext-align:center\;\nwhite-space:nowrap}\n";
   
  HTML +="</style>\n\n</head>\n\n<body>\n<h1>Baño Anti Manchas</h1>\n";

   if(dirStatus == 2){
    HTML +="\n\t<h2><span style=\"background-color: #FFFF00\">Forward</span></h2>\n";    
   }else if(dirStatus == 1){
    HTML +="\n\t<h2><span style=\"background-color: #FFFF00\">Reverse</span></h2>\n";      
   }else{
    HTML +="\n\t<h2><span style=\"background-color: #FFFF00\">OFF</span></h2>\n";    
   }
      if(dirStatus ==1){
        HTML +="\t<div class=\"btn\">\n\t\t<a class=\"angleButton\" style=\"background-color:#f56464\"  href=\"/motor?";
        HTML += argId[0];
        HTML += "=off\">";
        HTML +=buttonTitle1[0];
      }else{
        HTML +="\t<div class=\"btn\">\n\t\t<a class=\"angleButton \" style=\"background-color:#90ee90\"  href=\"/motor?";  
        HTML += argId[0];
        HTML += "=on\">";       
        HTML +=buttonTitle2[0];  
      }   
     HTML +="</a>\t\n\t</div>\n\n";  
     if(dirStatus ==2){
        HTML +="\t<div class=\"btn\">\n\t\t<a class=\"angleButton\" style=\"background-color:#f56464\"  href=\"/motor?";
        HTML += argId[1];
        HTML += "=off\">";
        HTML +=buttonTitle1[1]; 
     }else{
        HTML +="\t<div class=\"btn\">\n\t\t<a class=\"angleButton \" style=\"background-color:#90ee90\"  href=\"/motor?";  
        HTML += argId[1];
        HTML += "=on\">";       
        HTML +=buttonTitle2[1];
     }   
     HTML +="</a>\t\n\t</div>\n\n";     

  HTML +="\t\n</body>\n</html>\n";
  server.send(200, "text/html", HTML);  

}

void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);

}

void setup(void) {

   // se declaran los pines como salidas
   pinMode(pin1, OUTPUT);
   pinMode(pin2, OUTPUT);
   pinMode(pin3, OUTPUT);
   pinMode(pin4, OUTPUT);  

   // inicia la comunicación serial 
   Serial.begin(115200);
   Serial.println("Control Baño Antimanchas");

   // declara el módulo como estación
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);
   Serial.println("");

   // escribe puntos antes de lograr la conexión
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }

   // escribe en el monitor serie la información de la conexión web
   Serial.println("");
   Serial.print("Connected to: ");
   Serial.println(ssid);
   Serial.print("IP address: http://");
   Serial.println(WiFi.localIP());


   if (MDNS.begin("robojaxESP8266")) {
     Serial.println("MDNS responder started");
     Serial.println("access via http://robojaxESP8266");
   }

   server.on("/", handleRoot);
   server.on("/motor", HTTP_GET, motorControl); 
   server.onNotFound(handleNotFound);
   server.begin();
   Serial.println("HTTP server started");
}

void loop(void) {
   server.handleClient();
   MDNS.update();
   
if(dirStatus == 1){ 
   poleStep++; 
   driveStepper(poleStep);    
 }else if(dirStatus == 2){ 
   poleStep--; 
   driveStepper(poleStep);    
 }else{
   driveStepper(8);   
 }
 if(poleStep > 7){ 
   poleStep = 0; 
 } 
 if(poleStep < 0){ 
   poleStep = 7; 
 } 
 
 delay(1); 
}

void motorControl() {

    if(server.arg(argId[0]) == "on")
    {
      dirStatus = 1;
               
    }else if(server.arg(argId[0]) == "off"){
      dirStatus = 3; 
          
    }else if(server.arg(argId[1]) == "on"){
      dirStatus = 2; 
          
    }else if(server.arg(argId[1]) == "off"){
      dirStatus = 3;
          
    }  
    
  handleRoot();
}

void driveStepper(int c)
{
     digitalWrite(pin1, pole1[c]);  
     digitalWrite(pin2, pole2[c]); 
     digitalWrite(pin3, pole3[c]); 
     digitalWrite(pin4, pole4[c]);   
}
