#include <ArduinoWind.h>
#include <Ethernet.h>
#include <Wire.h>
#include "DFRobot_SHT20.h"
#include <Debouncer.h>

//#define DEBUG

//PINOS*****************
#define PINO_BIR 0
#define PINO_ANEM 2
#define PINO_SDCARD 4
//**********************

//CONSTANTES************
#define PI 3.14159265
#define PERIOD 12000
#define RADIUS 147
#define KMH2KNOTS 0.539957
#define TIME2SEND 30000
#define DEBOUNCE_DURATION_MS 50

#define MIN_CTE     1000

#define N_LEITURAS_ANEM 5

#define s_uid       "StationHPOA"
#define passphrase  "6gJKDQx75HdkCq3"
//**********************

//COMPONENTES***********
DFRobot_SHT20 sht20;
Station myStation(s_uid, passphrase, 5);
Debouncer debouncer(PINO_ANEM, DEBOUNCE_DURATION_MS);
//**********************

//TIPOS DE DADOS********
struct{
  float Temperatura;
  float Umidade;
}typedef SHT20_INF;

struct{
  float VelocidadeVentoKMH;
  float VelocidadeVentoKNOTS;
}typedef ANEM_INF;

struct{
  float Tensao;
  String DirecaoVento;
  int WindDirDegrees;
}typedef BIR_INF;

//**********************

int counter=0;
bool control=true;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//EthernetClient c;
//HttpClient http(c);


void setup() {
  Serial.begin(9600);

  //pinMode(PINO_SDCARD, OUTPUT);
  //digitalWrite(PINO_SDCARD, HIGH);
  if(!Ethernet.begin(mac)){
    Serial.println("Error getting IP address via DHCP");
  }
  pinMode(PINO_ANEM, INPUT_PULLUP);
  pinMode(PINO_BIR, INPUT);
  debouncer.subscribe(Debouncer::Edge::RISE, addcount);
  sht20.initSHT20();
}

void loop() {
  SHT20_INF SHT20;
  ANEM_INF ANEM;
  BIR_INF BIR;
  int leituras=1;
  float acc=0;
  
  myStation.set_wind_min(MIN_CTE);
  myStation.set_wind_max(0);
  myStation.set_wind_avg(0);

  while(leituras <= N_LEITURAS_ANEM){
    ANEM = leituraANEM();
    if(ANEM.VelocidadeVentoKMH < myStation.get_wind_min()) myStation.set_wind_min(ANEM.VelocidadeVentoKMH);
    if(ANEM.VelocidadeVentoKMH > myStation.get_wind_max()) myStation.set_wind_max(ANEM.VelocidadeVentoKMH);
    acc+= ANEM.VelocidadeVentoKMH;

    leituras++;
  }
  acc/= N_LEITURAS_ANEM;
  
  SHT20 = leituraSHT20();
  BIR = leituraBIR();

  myStation.set_temp(SHT20.Temperatura);
  myStation.set_rh(SHT20.Umidade);
  myStation.set_wind_avg(acc);
  myStation.set_wind_dir(BIR.WindDirDegrees);

  #ifdef DEBUG
    printaDados();
  #endif

  myStation.send_data();
}

SHT20_INF leituraSHT20(){
  SHT20_INF inf;

  inf.Umidade = sht20.readHumidity();
  inf.Temperatura = sht20.readTemperature();

  return inf;
};

ANEM_INF leituraANEM(){
  unsigned int RPM;
  ANEM_INF inf;

  windvelocity();
  RPM = RPMcalc();
  inf.VelocidadeVentoKMH = SpeedWind(RPM);
  
  return inf;
};

void windvelocity(){
  counter = 0;
  unsigned long millis();
  long startTime = millis();
  while(millis() < startTime + PERIOD){
    debouncer.update();
  }
};

unsigned int RPMcalc(){
  unsigned int RPM;
  return RPM=((counter)*60)/(PERIOD/1000);  // Calculate revolutions per minute (RPM)
};

float SpeedWind(unsigned int RPM){
  float speedwind;
  return speedwind = (((2 * PI * RADIUS * RPM)/60) / 1000)*3.6;  // Calculate wind speed on km/h
};

void addcount(){
  counter++;
  #ifdef DEBUG
    Serial.println("REED");
  #endif
};

BIR_INF leituraBIR(){
  BIR_INF inf;

  inf.Tensao = analogRead(PINO_BIR)*(5.0/1023.0);
  if (inf.Tensao > 0.40 && inf.Tensao < 0.70){
    inf.DirecaoVento = "Noroeste (NO)";   //NO
    inf.WindDirDegrees = 315;
  }else if (inf.Tensao > 1.0 && inf.Tensao < 1.4){
    inf.DirecaoVento = "Oeste (O)";       //O
    inf.WindDirDegrees = 270;
  }else if (inf.Tensao > 1.6 && inf.Tensao < 2.0){
    inf.DirecaoVento = "Sudoeste (SO)";   //SO
    inf.WindDirDegrees = 225;
  }else if (inf.Tensao > 2.2 && inf.Tensao < 2.5){
    inf.DirecaoVento = "Sul (S)";         //S
    inf.WindDirDegrees = 180;
  }else if (inf.Tensao > 2.8 && inf.Tensao < 3.2){
    inf.DirecaoVento = "Sudeste (SE)";    //SE
    inf.WindDirDegrees = 135;
  }else if (inf.Tensao > 3.3 && inf.Tensao < 3.8){
    inf.DirecaoVento = "Leste (E)";       //E
    inf.WindDirDegrees = 90;
  }else if (inf.Tensao > 3.9 && inf.Tensao < 4.3){
    inf.DirecaoVento = "Nordeste (NE)";   //NE
    inf.WindDirDegrees = 45;
  }else if(inf.Tensao > 4.5 && inf.Tensao < 4.9){
    inf.DirecaoVento = "Norte (N)";       //N
    inf.WindDirDegrees = 0;
  };

  return inf;
};

#ifdef DEBUG
void printaDados(){
  Serial.print("Temperatura:");
  Serial.print(myStation.get_temp());
  Serial.print(" Umidade:");
  Serial.print(myStation.get_rh());
  Serial.print(" Velocidade do Vento:");
  Serial.print(myStation.get_wind_avg());
  Serial.print(myStation.get_wind_max());
  Serial.print(myStation.get_wind_min());
  Serial.print(" Direção do Vento:");
  Serial.print(myStation.get_wind_dir());
};
#endif
