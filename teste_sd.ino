
#include <dht.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#include <SD.h>
int pinoSS = 53;

File myFile;

#define pinSensorTemperaturaSolo 3
#define pinSensorUmidadeSolo A0
#define pinSensorChuva A1
#define pinSensorTemperatura 8
#define pinSensorUV A2

void measure_UV();

const float pi = 3.14159265;     //Número de pi
int radius = 147;                //Raio do anemometro(mm)
unsigned int Sample  = 0;        //Armazena o número de amostras
unsigned int counter = 0;        //Contador para o sensor  
unsigned int RPM = 0;            //Rotações por minuto
float speedwind = 0;             //Velocidade do vento (km/h)
int adc_value = 0,
    UV_index  = 0;
unsigned int mv;

OneWire oneWire(pinSensorTemperaturaSolo);
DallasTemperature sensor(&oneWire);
dht sensorDHT;

void setup() {
  Serial.begin(9600);
  pinMode(pinoSS, OUTPUT);

  while (!Serial) {
    ;// espera que a porta serial se conecte. Necessário apenas para porta USB nativa
  }
  if (!SD.begin(4)) {
      Serial.println(F("falhou!"));
      while (1); 
      }
  myFilec = SD.open("data2.csv", FILE_WRITE);
  if (myFilec) {
        myFilec.println("pa_mx, pa_mi, pa_md, chu, t_ar_mx, t_ar_mi, t_ar_md, u_r_mx, u_r_mi, u_r_md, uv_in, rad_t, rad_md, rpm, ve_ar, t_mx_s, t_mi_s, t_md_s, um_s");
        myFilec.close();
        Serial.println(F("Cabeçalho feito")); 
        }
        
  pinMode(2, INPUT);        //configura o digital 2 como entrada
  digitalWrite(2, HIGH);    //internall pull-up active
  pinMode(pinSensorUmidadeSolo, INPUT);
  pinMode(pinSensorChuva, INPUT);
  pinMode(pinSensorUV, INPUT);
  sensor.begin();

}
 
  
void loop() {

  //int chk = sensorDHT.read22(pinSensorTemperatura);
  
  /* VERIFICA SE HOUVE ERRO
  switch (chk){
      case DHTLIB_OK:
          Serial.print(F("OK,\t"));
          break;
      case DHTLIB_ERROR_CHECKSUM:
          Serial.print(F("Checksum error,\t"));
          break;
      case DHTLIB_ERROR_TIMEOUT:
          Serial.print(F("Time out error,\t"));
          break;
      case DHTLIB_ERROR_CONNECT:
          Serial.print(F("Connect error,\t"));
          break;
      case DHTLIB_ERROR_ACK_L:
          Serial.print(F("Ack Low error,\t"));
          break;
      case DHTLIB_ERROR_ACK_H:
          Serial.print(F("Ack High error,\t"));
          break;
      default:
          Serial.print("Unknown error,\t");
          break;
      }*/
  float vet_precao[30];
  float vet_hum_ar[30];
  float vet_temp[30];
  float vet_temp_solo[30];
  float vet_radiacao[30];
  
  for (int i = 0; i < 30; ++i) {
      vet_temp[i] = sensorDHT.temperature, 1;
      vet_hum_ar[i] = sensorDHT.humidity, 1; 
      radiacao_UV();
      vet_radiacao[i] = mv;
      sensor.requestTemperatures();
      float leitura=sensor.getTempCByIndex(0);
      vet_temp_solo[i] = leitura; 
      delay(60000);
    }
    
  if (!SD.begin(4)) {
      Serial.println("falhou");
      while (1);
      }
      myFile = SD.open("data2.csv", FILE_WRITE);
    
  if (myFile) {
  Serial.println(F("Escrevendo "));
  myFile.print("0");
  myFile.print(", ");
  myFile.print("0");
  myFile.print(", ");
  myFile.print("0");
  myFile.print(", ");
  myFile.print(analogRead(pinSensorChuva)); 
  myFile.print(", ");
  maxmim(vet_temp);
  myFile.print(", ");
  maxmim(vet_hum_ar);
  myFile.print(", ");
  measure_UV();
  myFile.print(UV_index);
  myFile.print(", ");
  totmedia(vet_radiacao);
  myFile.print(", ");
  Sample++;
  windvelocity();
  RPMcalc();
  myFile.print(RPM);
  myFile.print(", ");
  SpeedWind();
  myFile.print(speedwind);
  myFile.print(" [km/h] ");
  myFile.print(", ");
  maxmim(vet_temp_solo);
  myFile.print(", ");
  myFile.print(analogRead(pinSensorUmidadeSolo));
  myFile.println("");
  
  myFile.close();
  Serial.println(F("done"));
  } 

}

// FUNÇÔES
void maxmim(float vetor[])
{
  float maior = 0;
  float menor = 9999;
  float media;
  float soma = 0;
  for (int i = 0; i < 30; ++i) {
     if (vetor[i] > maior) maior = vetor[i];
     if (vetor[i] < menor) menor = vetor[i];
     soma += vetor[i];
     delay(2);
    }
  media = soma / 30;

  myFile.print(maior);
  myFile.print(", ");
  myFile.print(menor);
  myFile.print(", ");
  myFile.print(media);
  
}

void totmedia(float vetor[])
{
  float media;
  float total = 0;
  for (int i = 0; i < 30; ++i) {
     total += vetor[i];
     delay(2);
    }
  media = total / 30;
  myFile.print(total);
  myFile.print(", ");
  myFile.print(media);
}

void windvelocity()
{
  speedwind = 0;
  
  counter = 0;  
  attachInterrupt(0, addcount, RISING);
  unsigned long millis();       
  long startTime = millis();
  while(millis() < startTime + 5000) {}
}

void RPMcalc()
{
  RPM=((counter)*60)/(5000/1000); 
}


void SpeedWind()
{
  speedwind = (((4 * pi * radius * RPM)/60) / 1000)*3.6; 
}

void addcount()
{
  counter++;
} 

void measure_UV()
{
  
    adc_value = analogRead(pinSensorUV);
    
    int mV = (adc_value * (5.0 / 1023.0)) * 1000;
  
    
    //Compara valores da tabela de índice UV
    if(mV > 0 && mV <= 227)   UV_index = 0;
    
    else if(mV > 227 && mV <= 318)  UV_index = 1;
    
    else if (mV > 318 && mV <= 408) UV_index = 2;
    
    else if (mV > 408 && mV <= 503) UV_index = 3;
    
    else if (mV > 503 && mV <= 606) UV_index = 4;
    
    else if (mV > 606 && mV <= 696) UV_index = 5;
    
    else if (mV > 696 && mV <= 795) UV_index = 6;
    
    else if (mV > 795 && mV <= 881) UV_index = 7;
    
    else if (mV > 881 && mV <= 976) UV_index = 8;
   
    else if (mV > 976 && mV <= 1079) UV_index = 9;
   
    else if (mV > 1079 && mV <= 1170) UV_index = 10;
    
    else if (mV > 1170) UV_index = 11;
}

void radiacao_UV()
{
    adc_value = analogRead(pinSensorUV);
    int mv = (adc_value * (5.0 / 1023.0)) * 1000;
}
