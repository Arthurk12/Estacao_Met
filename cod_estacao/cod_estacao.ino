#include <SPI.h>
#include <Ethernet.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <OneWire.h>
#include <TimeLib.h>
#include <MD5.h>
#include <EthernetClient.h>
#include <HttpClient.h>

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
#define DELAYTIME 9000
#define RADIUS 147
#define KMH2KNOTS 0.539957
#define TIME2SEND 30000

#define s_uid       "StationHPOA"
#define passphrase  "6gJKDQx75HdkCq3"

#define fixed_url   "www.windguru.cz"
//**********************

//COMPONENTES***********
#define DHTTYPE DHT21
DHT dht(PINO_DHT21, DHTTYPE);
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
  float VelocidadeVentoKMH;
  float VelocidadeVentoKNOTS;
}typedef ANEM_INF;

struct{
  float Tensao;
  String DirecaoVento;
  int WindDirDegrees;
}typedef BIR_INF;

struct{
  String salt;
  String hash;
}typedef MD5HASH;

struct{
  MD5HASH md5;
  String url;
  float MeanTemperature;
  float Humidity;
  ANEM_INF ANEM;
  BIR_INF BIR;
  bool val[4];
}typedef WINDGURU;
//**********************

int counter=0;
bool control=true;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

unsigned long t_ini = 0;
unsigned long t_end;

EthernetClient c;
HttpClient http(c);

void setup() {
  Serial.begin(9600);
  //pinMode(PINO_SDCARD, OUTPUT);
  //digitalWrite(PINO_SDCARD, HIGH);
  if(!Ethernet.begin(mac)){
    Serial.println("Error getting IP address via DHCP");
  }
  pinMode(PINO_ANEM, INPUT);
  digitalWrite(PINO_ANEM, HIGH);
  pinMode(PINO_BIR, INPUT);
  digitalWrite(PINO_DHT21, HIGH);
  DS18B20.begin();
  dht.begin();

}

void loop() {
  DHT21_INF DHT21_;
  DS18B20_INF DS18B20;
  ANEM_INF ANEM;
  BIR_INF BIR;
  bool estado;
  
  DHT21_ = leituraDHT21();
  DS18B20 = leituraDS18B20();
  ANEM = leituraANEM();
  BIR = leituraBIR();
  
  WINDGURU info = process_sensor_data(DHT21_, DS18B20, ANEM, BIR);
  check_data(&info);
  info.md5 = generate_hash();
  info.url = url_builder(info);
  
  t_end = millis();
  Serial.println(t_end - t_ini);
  if((t_end - t_ini)>= TIME2SEND || t_ini == 0){
    estado = send_data(info.url);
    t_ini = t_end;
  }
  
  //Descomentar esta funcao para debugar o programa (o corpo dela tambem esta comentado)
  printaDados(info, estado);
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
  inf.VelocidadeVentoKMH = SpeedWind(RPM);
  
  return inf;
};

void windvelocity(){
  counter = 0;
  attachInterrupt(digitalPinToInterrupt(PINO_ANEM), addcount, RISING);
  //unsigned long millis();
  long startTime = millis();
  while(millis() < startTime + PERIOD){}
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
};

BIR_INF leituraBIR(){
  BIR_INF inf;

  inf.Tensao = analogRead(PINO_BIR)*(5.0/1023.0);
  if (inf.Tensao <= 0.26){
    inf.DirecaoVento = "Noroeste (NO)";   //NO
    inf.WindDirDegrees = 315;
  }else if (inf.Tensao <= 0.30){
    inf.DirecaoVento = "Oeste (O)";       //O
    inf.WindDirDegrees = 270;
  }else if (inf.Tensao <= 0.35){
    inf.DirecaoVento = "Sudoeste (SO)";   //SO
    inf.WindDirDegrees = 225;
  }else if (inf.Tensao <= 0.42){
    inf.DirecaoVento = "Sul (S)";         //S
    inf.WindDirDegrees = 180;
  }else if (inf.Tensao <= 0.51){
    inf.DirecaoVento = "Sudeste (SE)";    //SE
    inf.WindDirDegrees = 135;
  }else if (inf.Tensao <= 0.66){
    inf.DirecaoVento = "Leste (E)";       //E
    inf.WindDirDegrees = 90;
  }else if (inf.Tensao <= 0.94){
    inf.DirecaoVento = "Nordeste (NE)";   //NE
    inf.WindDirDegrees = 45;
  }else{
    inf.DirecaoVento = "Norte (N)";       //N
    inf.WindDirDegrees = 0;
  };

  return inf;
};


WINDGURU process_sensor_data(DHT21_INF DHT21_, DS18B20_INF DS18B20, ANEM_INF ANEM, BIR_INF BIR){
  WINDGURU all_info;

  if(DS18B20.Temperatura != -127 && !isnan(DHT21_.Temperatura)){
    all_info.MeanTemperature = (int) round(((DHT21_.Temperatura + DS18B20.Temperatura)/2));
  }else if(!isnan(DHT21_.Temperatura)){
    all_info.MeanTemperature = (int) round((DHT21_.Temperatura));
  }else if(DS18B20.Temperatura != -127){
    all_info.MeanTemperature = (int) round((DS18B20.Temperatura));
  }
  all_info.Humidity = DHT21_.Umidade;
  all_info.ANEM.VelocidadeVentoKMH = (int)ANEM.VelocidadeVentoKMH;
  all_info.ANEM.VelocidadeVentoKNOTS = all_info.ANEM.VelocidadeVentoKMH * KMH2KNOTS;
  all_info.BIR = BIR;

  return all_info;
};

MD5HASH generate_hash(){
  MD5HASH md5;
  String temp;
  
  randomSeed(analogRead(0));
  String salt = String(random(233, 10009));
  temp.concat(salt);
  temp.concat(s_uid);
  temp.concat(passphrase);

  //generate the MD5 hash for our string
  unsigned char* hash=MD5::make_hash(temp.c_str());
  //generate the digest (hex encoding) of our hash
  char *md5str = MD5::make_digest(hash, 16);

  md5.hash = String(md5str);
  md5.salt = salt; 
  free(hash);
  free(md5str);
  return md5;
};

String url_builder(WINDGURU info){
  String url;
  url.concat("/upload/api.php?uid=");
  url.concat(s_uid);
  url.concat("&salt=");
  url.concat(info.md5.salt);
  url.concat("&hash=");
  url.concat(info.md5.hash);
  url.concat("&interval=");
  url.concat(TIME2SEND/1000);
  if(info.val[2]){
    url.concat("&wind_avg=");
    url.concat(info.ANEM.VelocidadeVentoKNOTS);
  }
  if(info.val[3]){
    url.concat("&wind_direction=");
    url.concat(info.BIR.WindDirDegrees);
  }
  if(info.val[0]){
    url.concat("&temperature=");
    url.concat(info.MeanTemperature);
  }
  if(info.val[1]){
    url.concat("&rh=");
    url.concat(info.Humidity);
  }
  return url;
};

bool send_data(String url_with_variables){
  int err =0;
  bool ret;

  err = http.get(fixed_url, String(url_with_variables).c_str());
  if (err == 0){
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err ==200){
      ret = 1;
    }else{    
      ret =  0;
    }
  }else{
    Serial.print("Connect failed: ");
    Serial.println(err);
    ret = 0;
  }
  http.stop();
  return ret;
};

void check_data(WINDGURU *info){
  if(!isnan(info->MeanTemperature) && (-10 < info->MeanTemperature && info->MeanTemperature < 60)){
    info->val[0] = 1;
  }else{
    info->val[0]=0;
  }

  if(!isnan(info->Humidity)){
    info->val[1] = 1;
  }else{
    info->val[1]=0;
  }

  if(!isnan(info->ANEM.VelocidadeVentoKNOTS) && (0 <= info->ANEM.VelocidadeVentoKNOTS)){
    info->val[2] = 1;
  }else{
    info->val[2]=0;
  }

  if(!isnan(info->BIR.WindDirDegrees) && (0 <= info->BIR.WindDirDegrees && info->BIR.WindDirDegrees <= 315)){
    info->val[3] = 1;
  }else{
    info->val[3]=0;
  }
};

void printaDados(WINDGURU info, bool sucesso){
  Serial.print("----------------------------------------");
  Serial.println();
  Serial.print("DHT21:   ");
  Serial.print("Temperatura Média: ");
  Serial.print(info.MeanTemperature);
  Serial.print(" C");
  Serial.print("    ");
  Serial.print("Umidade: ");
  Serial.print(info.Humidity);
  Serial.print(" %");
  Serial.println();
  Serial.print("ANEMOMETRO:   ");
  Serial.print("Velocidade Vento: ");
  Serial.print(info.ANEM.VelocidadeVentoKMH);
  Serial.print(" km/h         ");
  Serial.print("Velocidade Vento: ");
  Serial.print(info.ANEM.VelocidadeVentoKNOTS);
  Serial.print(" knots");
  Serial.println();
  Serial.print("BIRUTA:   ");
  Serial.print("Direcao Vento: ");
  Serial.print(info.BIR.DirecaoVento);
  Serial.print("     Direcao Vento(GRAUS): ");
  Serial.print(info.BIR.WindDirDegrees);
  Serial.print("    ");
  Serial.print("Tensao: ");
  Serial.print(info.BIR.Tensao);
  Serial.print(" ");
  Serial.println();
  Serial.print("URL:   ");
  Serial.print(info.url);
  Serial.println();
  Serial.print("HASH:   ");
  Serial.print(info.md5.hash);
  Serial.print("    ");
  Serial.print("salt: ");
  Serial.print(info.md5.salt);
  Serial.print(" ");
  Serial.println();
  if(sucesso){
    Serial.println("Dados enviados com sucesso! =)");
  }else{
    Serial.println("Falha ao enviar dados! =(");
  }
  Serial.println("----------------------------------------");
};
