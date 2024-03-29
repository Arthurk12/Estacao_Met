#include <ArduinoWind.h>
#include <Ethernet.h>
#include <Wire.h>
#include "DFRobot_SHT20.h"

//PAINEL DE CONTROLE****
#define DEBUG
//#define SEND_DATA
//**********************

//PINOS*****************
#define PINO_BIR                0
#define PINO_ANEM               2
#define PINO_SDCARD             4
//**********************

//CONSTANTES************
#define PI                      3.14159265
#define RADIUS_IN_METERS        0.147
#define MS_2_KMH                3.6
#define MS_2_KNOTS              1.94384449
#define PERIOD_IN_MS            5000
#define N_LEITURAS_ANEM         6
#define TIME_2_SEND_IN_SECONDS  PERIOD_IN_MS * N_LEITURAS_ANEM / 1000

#define guru_uid                ""
#define guru_passphrase         ""
#define windy_uid               ""
//**********************

//COMPONENTES***********
DFRobot_SHT20 sht20;
Station myStation(guru_uid, guru_passphrase, TIME_2_SEND_IN_SECONDS);
//**********************

//TIPOS DE DADOS********
struct{
  float Temperatura;
  float Umidade;
}typedef SHT20_INF;

struct{
  float VelocidadeVentoMS;
  float VelocidadeVentoKMH;
  float VelocidadeVentoKNOTS;
}typedef ANEM_INF;

struct{
  float Tensao;
  String DirecaoVento;
  int WindDirDegrees;
}typedef BIR_INF;

//**********************

volatile unsigned int rotations;
volatile unsigned long contactBounceTime;
bool control=true;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


void setup() {
  Serial.begin(9600);
  
  #ifdef SEND_DATA
  if(!Ethernet.begin(mac)){
    Serial.println("Error getting IP address via DHCP");
  }else{
    Serial.print("IP address assigned: ");
    Serial.println(Ethernet.localIP());
  }
  #endif
  
  pinMode(PINO_ANEM, INPUT_PULLUP);
  pinMode(PINO_BIR, INPUT);
  sht20.initSHT20();
}

void loop() {
  SHT20_INF SHT20;
  ANEM_INF ANEM;
  BIR_INF BIR;
  int leituras=1;

  while(leituras <= N_LEITURAS_ANEM){
    ANEM = leituraANEM();
    myStation.set_wind_velocity(ANEM.VelocidadeVentoKNOTS);
    leituras++;
  }
  
  SHT20 = leituraSHT20();
  BIR = leituraBIR();

  myStation.set_temp(SHT20.Temperatura);
  myStation.set_rh(SHT20.Umidade);

  if(BIR.WindDirDegrees >=0 && BIR.WindDirDegrees <= 360) myStation.set_wind_dir(BIR.WindDirDegrees);

  #ifdef DEBUG
  printaDados();
  #endif

  #ifdef SEND_DATA
  myStation.send_data();
  #endif
}

SHT20_INF leituraSHT20(){
  SHT20_INF inf;

  inf.Umidade = sht20.readHumidity();
  inf.Temperatura = sht20.readTemperature();

  return inf;
};

ANEM_INF leituraANEM(){
  rotations = 0;
  ANEM_INF inf;
  
  attachInterrupt(digitalPinToInterrupt(PINO_ANEM), rotationCount, RISING);

  delay(PERIOD_IN_MS);

  detachInterrupt(digitalPinToInterrupt(PINO_ANEM));

  float periodInSeconds = PERIOD_IN_MS/1000;
  float velocidadeAngular = rotations * ((2 * PI) / periodInSeconds);



  inf.VelocidadeVentoMS = velocidadeAngular * RADIUS_IN_METERS;
  inf.VelocidadeVentoKMH = inf.VelocidadeVentoMS * MS_2_KMH;
  inf.VelocidadeVentoKNOTS = inf.VelocidadeVentoMS * MS_2_KNOTS;

  #ifdef DEBUG
  Serial.print(inf.VelocidadeVentoKMH); Serial.print("Km/h "); Serial.print(inf.VelocidadeVentoMS); Serial.print("M/s "); Serial.print(inf.VelocidadeVentoKNOTS); Serial.print("Knots "); Serial.print("\t\t");
  Serial.print(rotations); Serial.println(" rotations");
  #endif
  
  return inf;
};

void rotationCount(){
  if ((millis() - contactBounceTime) > 15 ) { // Debounce the switch contact
                                              // 15 miliseconds dellimit max speed at about 220km/h
    rotations++;
    contactBounceTime = millis();
    
    #ifdef DEBUG
    Serial.println("REED");
    #endif
  }
};

BIR_INF leituraBIR(){
  BIR_INF inf;

  inf.Tensao = analogRead(PINO_BIR)*(5.0/1023.0);
  if (inf.Tensao >= 0.40 && inf.Tensao < 0.85){
    inf.DirecaoVento = "Noroeste (NO)";   //NO
    inf.WindDirDegrees = 315;
  }else if (inf.Tensao >= 0.85 && inf.Tensao < 1.5){
    inf.DirecaoVento = "Oeste (O)";       //O
    inf.WindDirDegrees = 270;
  }else if (inf.Tensao >= 1.5 && inf.Tensao < 2.1){
    inf.DirecaoVento = "Sudoeste (SO)";   //SO
    inf.WindDirDegrees = 225;
  }else if (inf.Tensao >= 2.1 && inf.Tensao < 2.65){
    inf.DirecaoVento = "Sul (S)";         //S
    inf.WindDirDegrees = 180;
  }else if (inf.Tensao > 2.65 && inf.Tensao <= 3.2){
    inf.DirecaoVento = "Sudeste (SE)";    //SE
    inf.WindDirDegrees = 135;
  }else if (inf.Tensao > 3.2 && inf.Tensao <= 3.8){
    inf.DirecaoVento = "Leste (E)";       //E
    inf.WindDirDegrees = 90;
  }else if (inf.Tensao > 3.8 && inf.Tensao < 4.4){
    inf.DirecaoVento = "Nordeste (NE)";   //NE
    inf.WindDirDegrees = 45;
  }else if(inf.Tensao >= 4.4 && inf.Tensao <= 4.9){
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
  Serial.println(myStation.get_rh());
  Serial.print("Velocidade do Vento:");
  Serial.print(myStation.get_wind_avg());
  Serial.print(" Max:");
  Serial.print(myStation.get_wind_max());
  Serial.print(" Min:");
  Serial.println(myStation.get_wind_min());
  Serial.print("Direção do Vento:");
  Serial.println(myStation.get_wind_dir());
};
#endif
