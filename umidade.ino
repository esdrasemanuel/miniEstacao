#define pinSensorUmidadeSolo A0

void setup() {
  pinMode(pinSensorUmidadeSolo, INPUT);
  Serial.begin(9600);
}

void loop() {

  int marcador = 0;
  float vet_hum_ar[30];
  float vet_hum_solo[30];
  float vet_temp[30];
  
  
  while (marcador < 30) {
    
  vet_hum_solo[marcador] = analogRead(pinSensorUmidadeSolo);

     Serial.print("   ");
     Serial.print(vet_hum_solo[marcador]);
     Serial.print("   ");

  delay(1000);
  
  marcador++;
  
  }
  
  float maior = 0;
  float menor = 9999;
  float media;
  float soma = 0;
  for (int i = 0; i < 30; ++i) {
     if (vet_hum_solo[i] > maior) maior = vet_hum_solo[i];
     if (vet_hum_solo[i] < menor) menor = vet_hum_solo[i];
     soma += vet_hum_solo[i];
     delay(5);
    }
  
  media = soma / 30;
  Serial.print("  media: ");
  Serial.print(media);
  Serial.print("  menor: " );
  Serial.println(menor);
  Serial.print("  maior: " );
  Serial.println(maior);
}
