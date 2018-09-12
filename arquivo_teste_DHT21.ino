#include <DHT.h>

#define DHTPIN 6                                    // Pino conectado ao pino de dados do sensor
#define DHTTYPE DHT21                              // Sensor DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);                         // Definicoes do sensor : pino, tipo

float hmin=999;
float hmax=0;

void setup() 
{
  Serial.begin(9600); 
  // Iniclaiza o sensor DHT
  dht.begin();
}

void UMIDADE(){
   float h = dht.readHumidity();                 // Leitura da umidade


  if(h < hmin){
    
    hmin = h;
    
  }else if(h > hmax){

    hmax = h;
  }

   Serial.print("Umidade : "); 
   Serial.print(h);
   Serial.println(" %");
   Serial.print(hmax);
   Serial.println(" %");
   Serial.print(hmin);
   Serial.println(" %");
}

void loop() 
{
  UMIDADE();
  
}
