
#include "WiFiEsp.h"
#include <SPI.h>
#include <SD.h>

#include <SoftwareSerial.h>
SoftwareSerial esp8266(10, 11); // RX, TX

File myFile;

char ssid[] = "estacao1";         // your network SSID (name)
char pass[] = "12345678";        // your network password
byte reqCount = 0;                // number of requests received

WiFiEspServer server(80);

RingBuffer buf(10);

void setup(){
  int status = WL_IDLE_STATUS;     // the Wifi radio's status

  Serial.begin(115200);   // initialize serial for debugging
  esp8266.begin(9600);    // initialize serial for ESP module
  WiFi.init(&esp8266);    // initialize ESP module

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("WiFi s not present"));
    while (true); 
  }

  Serial.print(F("start AP "));

  IPAddress localIp(192, 168, 4, 1);
  WiFi.configAP(localIp);

  status = WiFi.beginAP(ssid, 10, pass, ENC_TYPE_WPA2_PSK);
  server.begin();
  Serial.println(F("Server started"));
}

void loop()
{

  WiFiEspClient client = server.available();

  if (client) {                             
    Serial.println(F("New client"));             
    buf.init();
    while (client.connected()) {     
      if (client.available()) {               
        char c = client.read();               
        buf.push(c);                          
        if (buf.endsWith("\r\n\r\n")) {
          //sendHttpResponse(client);
          break;
        }
        
        if(buf.endsWith("GET /data.csv")){
          Serial.print(F("Initializing"));
          if (!SD.begin(4)) {
              Serial.println(F("falhou"));
              while (1);
              }
           Serial.println(F("done"));    
           myFile = SD.open("data.csv");
           if (myFile) {
              while (myFile.available()) {      
                     client.println(F("HTTP/1.1 200 OK\n"
                     "Content-Type: text/csv\n"
                     "Content-Disposition: attachment; filename=\"data.csv\"\n"
                     "Connection: close\n"
                     ));
                     client.println(myFile, "application/octet-stream");
                     myFile.close();
                    }
           
            }
         }
      
    }
    
 }
    // give the web browser time to receive the data
    delay(10);

    // close the connection
    client.stop();
    Serial.println(F("Client disconnected"));
  }
}

void sendHttpResponse(WiFiEspClient client)
{

  client.print(F("<!DOCTYPE HTML>\r\n"));
  client.print(F("<html>\r\n"));
  client.print(F("<h1>Hello World!</h1>\r\n"));
  client.print(F("</html>\r\n"));
}
