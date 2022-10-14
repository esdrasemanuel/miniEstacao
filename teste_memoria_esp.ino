
#include <WiFiEsp.h>

#ifndef HAVE_HWSERIAL1
#include <SoftwareSerial.h>
SoftwareSerial esp8266(2, 3); // RX, TX
#endif

File wFile;
char ssid[] = "esta";  
char pass[] = "12345678";      
int status = WL_IDLE_STATUS;     
int reqCount = 0;                

WiFiEspServer server(80);
RingBuffer buf(8);

void setup()
{
  Serial.begin(115200);
  esp8266.begin(9600);  
  WiFi.init(&esp8266);   

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(F("WiFi shield not present"));
    while (true); 
  }

  Serial.print(F("start AP "));
  Serial.println(ssid);

  IPAddress localIp(192, 168, 1, 1);
  WiFi.configAP(localIp);
 
  status = WiFi.beginAP(ssid, 10, pass, ENC_TYPE_WPA2_PSK);

  Serial.println(F("Access point started"));
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
          sendHttpResponse(client);
          break;
        }
      }
    }
    
    delay(10);
    client.stop();
    Serial.println(F("Client disconnected"));
  }
}

void sendHttpResponse(WiFiEspClient client)
{
  client.print(F("HTTP/1.1 200 OK\n"
                   "Content-Type: application/download\n"
                   "Connection: close\n"));
  client.print(F("<!DOCTYPE HTML>\r\n"));
  client.print(F("<html>\r\n"));
  client.print(F("<h1>Hello World!</h1>\r\n"));
  client.print(F("Requests received: "));
  client.print(++reqCount);
  client.print(F("<br>\r\n"));
  client.print(F("Analog input A0: "));
  client.print(analogRead(0));
  client.print(F("<br>\r\n"));
  client.print(F("</html>\r\n"));
}

void createFile(void){
  
 
  //Cria o arquivo se ele não existir
  if(SPIFFS.exists("/log.txt")){
    Serial.println("Arquivo ja existe!");
  } else {
    Serial.println("Criando o arquivo...");
    wFile = SPIFFS.open("/log.txt","w+");
 
    //Verifica a criação do arquivo
    if(!wFile){
      Serial.println("Erro ao criar arquivo!");
    } else {
      Serial.println("Arquivo criado com sucesso!");
    }
  }
  wFile.close();
}
