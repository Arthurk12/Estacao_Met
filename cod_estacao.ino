#define BLYNK_PRINT Serial
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <OneWire.h>

//PINOS*****************
#define PINO_BIR 0
#define PINO_ANEM 2
#define PINO_SDCARD 4
#define PINO_DHT21 5
#define PINO_DS18B20 6
//**********************

//CONSTANTES************
#define PI 3.14159265
#define PERIOD 5000
#define DELAYTIME 2000
#define RADIUS 147

char auth[] = "827e46fe2ed24967858e0afb23b0746f";
//**********************

//COMPONENTES***********
#define DHTTYPE DHT21
DHT dht(PINO_DHT21, DHTTYPE);
//BlynkTimer timer;
OneWire One_DS18B20(PINO_DS18B20);
DallasTemperature DS18B20(&One_DS18B20);
//**********************

//TIPOS DE DADOS********
struct{
  float Temperatura;
  float Umidade;
}typedef DHT21_INF;

struct{
  float Temperatura;
}typedef DS18B20_INF;

struct{
  float VelocidadeVento;
}typedef ANEM_INF;

struct{
  String DirecaoVento;
}typedef BIR_INF;
//**********************

//**********************
void printaDados(DHT21_INF DHT21_, DS18B20_INF DS18B20, ANEM_INF ANEM, BIR_INF BIR);
//**********************
int counter=0;


void setup() {
  Serial.begin(9600);
  pinMode(PINO_SDCARD, OUTPUT);
  digitalWrite(PINO_SDCARD, HIGH);
  pinMode(PINO_ANEM, INPUT);
  digitalWrite(PINO_ANEM, HIGH);
  pinMode(PINO_BIR, INPUT);
  DS18B20.begin();
  dht.begin();
  
  Blynk.begin(auth);
//  timer.setInterval(1000L, EnviaDados);
}

void loop() {
  DHT21_INF DHT21_;
  DS18B20_INF DS18B20;
  ANEM_INF ANEM;
  BIR_INF BIR;
  bool sucesso;
  
  Blynk.run();
//  timer.run();
  
  DHT21_ = leituraDHT21();
  DS18B20 = leituraDS18B20();
  ANEM = leituraANEM();
  BIR = leituraBIR();
  sucesso = EnviaDados(DHT21_, DS18B20, ANEM, BIR);
  //Descomentar esta funcao para debugar o programa (o corpo dela tambem esta comentado)
  printaDados(DHT21_, DS18B20, ANEM, BIR, sucesso);
  delay(DELAYTIME);
}

DHT21_INF leituraDHT21(){
  DHT21_INF inf;

  inf.Umidade = dht.readHumidity();
  inf.Temperatura = dht.readTemperature();

  return inf;
};

DS18B20_INF leituraDS18B20(){
  DS18B20_INF inf;

  
  DS18B20.requestTemperatures();
  
  inf.Temperatura = DS18B20.getTempCByIndex(0);
  
  return inf;
};

ANEM_INF leituraANEM(){
  unsigned int RPM;
  ANEM_INF inf;

  windvelocity();
  RPM = RPMcalc();
  inf.VelocidadeVento = SpeedWind(RPM);
  
  return inf;
};

void windvelocity(){
  counter = 0;
  attachInterrupt(digitalPinToInterrupt(PINO_ANEM), addcount, RISING);
  unsigned long millis();
  long startTime = millis();
  while(millis() < startTime + PERIOD){}
};

unsigned int RPMcalc(){
  unsigned int RPM;
  return RPM=((counter)*60)/(PERIOD/1000);  // Calculate revolutions per minute (RPM)
};

float SpeedWind(unsigned int RPM){
  float speedwind;
  return speedwind = (((4 * PI * RADIUS * RPM)/60) / 1000)*3.6;  // Calculate wind speed on km/h
};

void addcount(){
  counter++;
};


BIR_INF leituraBIR(){
  BIR_INF inf;
  int valor;

  valor = analogRead(PINO_BIR)*(5.0/1023.0);
  if (valor <= 0.27){
    inf.DirecaoVento = "Noroeste (NO)";  //NO
  }else if (valor <= 0.32){
    inf.DirecaoVento = "Oeste (O)";  //O
  }else if (valor <= 0.38){
    inf.DirecaoVento = "Sudoeste (SO)";  //SO
  }else if (valor <= 0.45){
    inf.DirecaoVento = "Sul (S)";  //S
  }else if (valor <= 0.57){
    inf.DirecaoVento = "Sudeste (SE)";  //SE
  }else if (valor <= 0.75){
    inf.DirecaoVento = "Leste (E)";   //E
  }else if (valor <= 1.25){
    inf.DirecaoVento = "Nordeste (NE)";   //NE
  }else{
    inf.DirecaoVento = "Norte (N)";  //N
  };

  return inf;
};

int EnviaDados(DHT21_INF DHT21_, DS18B20_INF DS18B20, ANEM_INF ANEM, BIR_INF BIR){
  Blynk.virtualWrite(V1, BIR.DirecaoVento);
  Blynk.virtualWrite(V3, ANEM.VelocidadeVento);
  Blynk.virtualWrite(V6, DHT21_.Umidade);
  Blynk.virtualWrite(V7, ((DHT21_.Temperatura + DS18B20.Temperatura)/2));

  return 1;
  
};

void printaDados(DHT21_INF DHT21_, DS18B20_INF DS18B20, ANEM_INF ANEM, BIR_INF BIR, int sucesso){
  Serial.print("----------------------------------------");
  Serial.println();
  Serial.print("DHT21:   ");
  Serial.print("Temperatura: ");
  Serial.print(DHT21_.Temperatura);
  Serial.print(" C");
  Serial.print("    ");
  Serial.print("Umidade: ");
  Serial.print(DHT21_.Umidade);
  Serial.print(" %");
  Serial.println();
  Serial.print("DS18B20:   ");
  Serial.print("Temperatura: ");
  Serial.print(DS18B20.Temperatura);
  Serial.print(" C");
  Serial.println();
  Serial.print("ANEMOMETRO:   ");
  Serial.print("Velocidade Vento: ");
  Serial.print(ANEM.VelocidadeVento);
  Serial.print(" km/h");
  Serial.println();
  Serial.print("BIRUTA:   ");
  Serial.print("Direcao Vento: ");
  Serial.print(BIR.DirecaoVento);
  Serial.print(" ");
  Serial.println();
  if(sucesso){
    Serial.println("Dados enviados para o aplicativo com sucesso! =)");
  }else{
    Serial.println("Falha ao enviar dados ao aplicativo! =(");
  }
  Serial.println("----------------------------------------");
};
