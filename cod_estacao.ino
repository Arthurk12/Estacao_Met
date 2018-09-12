#define BLYNK_PRINT Serial
#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <DHT.h>
#include <DallasTemperature.h>
#include <OneWire.h>

char auth[] = "2270362d14ae4b5e9508a6aacb8d7c8b";

#define WS100_CS 10
#define SDCARD_CS 4
#define DHTPIN 5                  // Pino do Sensor DHT11/21/22
#define DHTTYPE DHT21            // Define o Tipo de Sensor
#define ONE_WIRE_BUS 6          // Define o Pino do Termometro
#define Hall sensor 2          // Define o Pino do Anemometro

//BIRUTA*************************************
//Definicoes de Constantes
int pin = 0;                  // Define o Pino da Biruta
float valor = 0;
int Winddir = 0;
//*******************************************

//ANEMOMETRO**********************************
//Definicoes de Constantes
const float pi = 3.14159265;  // Numero Pi
int period = 5000;           // Tempo de Medida (milisegundos)
int delaytime = 2000;       // Tempo entre Medidas (milisegundos)
int radius = 147;          // Raio do Anemometro (milimetros)

//Definicoes de Variaveis
unsigned int Sample = 0;      // Sample Number
unsigned int counter = 0;    // Magnet Counter for Sensor
unsigned int RPM = 0;       // Rotacoes por minuto
float windspeed = 0;       // Wind Speed (km/h)
float speedwind = 0;         // Wind speed (m/s)
//********************************************

//TERMOMETRO**********************************
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;

float tempMin=999;
float tempMax=0;
float tempC=0;

void Temperature();    //Chama a funcao para medir a Temperatura
//********************************************

//DHT21***************************************
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Falha ao Ler Dados do Sensor!");
    return;
    }
    Blynk.virtualWrite(V6, h);              // Passa o Data de Umidade para o Medidor do App
  }
//*********************************************

// Voce pode enviar qualquer valor a qualquer momento.
// Por favor nao envie mais de 10 valores por segundo.





void setup() {
  //Debug Console
  Serial.begin(9600);

  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH); // Deselect the SD card
  pinMode(2, INPUT);            // Seta o Pino 2 como Entrada (Anemometro)
  digitalWrite(2, HIGH);       // Seta o Pino como Pull-up (Anemometro)

  Blynk.begin(auth);
  dht.begin();
  timer.setInterval(1000L, sendSensor);

}

void loop() {
  Blynk.run();
  timer.run();

//BIRUTA**************************************
valor = analogRead(pin)* (5.0 / 1023.0);

Serial.print("leitura do sensor :");
Serial.print(valor);
Serial.print(" volt");

if (valor <= 0.27){
  Winddir = 315;  //NO
}else if (valor <= 0.32){
  Winddir = 270;  //O
}else if (valor <= 0.38){
  Winddir = 225;  //SO
}else if (valor <= 0.45){
  Winddir = 180;  //S
}else if (valor <= 0.57){
  Winddir = 135;  //SE
}else if (valor <= 0.75){
  Winddir = 90;   //E
}else if (valor <= 1.25){
  Winddir = 45;   //NE
}else{
  Winddir = 000; //N
}

Serial.print("Direcao a :");
Serial.print(Winddir);
Blynk.virtualWrite(A0, Winddir);     // Passa o Data da Birutra para o App (++ PODE DAR ERRO AKI ++)
Serial.print(" graus");
Serial.println();
delay (1000);
//********************************************

//ANEMOMETRO**********************************
Sample++;
Serial.print(Sample);
Serial.print(": Start measurement...");
windvelocity();
Serial.println("  Finished.");
Serial.print("Counter: ");
Serial.print(counter);
Serial.print("; RPM: ");
RPMcalc();
Serial.print(RPM);
Serial.print("; Wind speed: ");
SpeedWind();
Serial.print(speedwind);
Serial.print(" [km/h] ");
Serial.print("");

delay(delaytime);       //Delay entre os Prints
//********************************************
}



//Medindo a Temperatura************************
//Le as Infos do Sensor
void Temperature(){
  sensors.requestTemperatures();
  tempC = sensors.getTempC(sensor1);
  Blynk.virtualWrite(V7, tempC);        // Passa o Data de Temperatura para o App
}
//*********************************************

//Medindo a Velocidade do Vento****************
void windvelocity(){
  speedwind = 0;
  counter = 0;
  attachInterrupt(0, addcount, RISING);
  unsigned long millis();
  long startTime = millis();
  while(millis() < startTime + period){}
  }
void RPMcalc(){
  RPM=((counter)*60)/(period/1000);     // Calcula as Rotacoes por minuto
  }
void SpeedWind(){
  speedwind = (((4 * pi * radius * RPM)/60) / 1000)*3,6;   // Calcula a Velocidade do Vento em KM/h
  Blynk.virtualWrite(V3, speedwind);                      // Passa o Data de Velocidade do Vento para o App
  }
void addcount(){
  counter++;
  }
//********************************************
