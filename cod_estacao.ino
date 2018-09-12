#include <DHT.h>                                   //Biblioteca do Sensor de Umidade
#include <Ethernet.h>                             //Biblioteca do Ethernet Shield
#include <SPI.h>                                 //Biblioteca da Interface Periferica Serial
#include <DallasTemperature.h>                  //Biblioteca de Temperaturas do Sensor DS18B20
#include <OneWire.h>                           //Biblioteca de Comunicação do Sensor DS18B20

byte mac [ ] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };      //Define o MAC ADDRESS do Ethernet Shield
IPAddress ip(192,168,0,122);                               //Define IP
IPAddress gateway(192,168,0,1);                           //Define GATEWAY
IPAddress subnet(255, 255, 255, 0);                      //Define MASCARA DE REDE

EthernetServer server (81);                   //Inicia o SERVIDOR WEB na PORTA (81)




// Pin definitions
#define LED 9                    //pino digial 9 (LED)
#define DHTPIN 6                   //pino digital 6(DHT)
#define ONE_WIRE_BUS 5         //Pino digital 4(termometro)
#define Hall sensor 2         //Pino digital 2(anemometro)

// Constants definitions
#define pi 3.14159265           // Numero pi
#define period 5000               // Tempo de medida(miliseconds)
#define delaytime 1000             // Time between samples (miliseconds)
#define radius 147      // Raio do anemometro(mm)


//SENSOR DE UMIDADE DEFINITIONS***********************************
#define DHTTYPE DHT21         //sensor DHT 21
DHT dht(DHTPIN, DHTTYPE);
//****************************************************************

//DECLARAÇÃO DE FUNCOES
void windvelocity();
void RPMcalc();
void SpeedWind();
void addcount();
void calcumidade();

//DECLARAÇÃO DE VARIAVEIS
float tempMin = 999;
float tempMax = 0;

float tempC=0;

unsigned int counter = 0; // magnet counter for sensor
unsigned int RPM = 0;          // Revolutions per minute
float speedwind = 0;         // Wind speed (m/s)
float windspeed = 0;           // Wind speed (km/h)
float maxspeed=0;             //maior velocidade do vento[Km/h]
float umidade=0;              //umidade
float umidademax=0;           //umidade maxima
float umidademin=0;           //umidade minima

OneWire oneWire (ONE_WIRE_BUS);

DallasTemperature sensors (&oneWire);
DeviceAddress sensor1;


void setup() {

  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();

//****************************************************************
//TERMOMETRO******************************************************
  //Start serial 
  Serial.begin(9600);                                     //CASO FALTE ESPAÇO, REVER ESTA PARTE DO CODIGO.
  sensors.begin();
  // Localiza e mostra enderecos dos sensores
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
  if (!sensors.getAddress(sensor1, 0)) 
     Serial.println("Sensores nao encontrados !"); 
//****************************************************************
//LED*************************************************************
  pinMode(LED, OUTPUT);
//****************************************************************
//ANEMOMETRO******************************************************
// Set the pins
  pinMode(Hall sensor, INPUT);
  digitalWrite(Hall sensor, HIGH);     //internall pull-up active
//****************************************************************

//SENSOR DE UMIDADE***********************************************
dht.begin();
//****************************************************************
}

void loop() {
//****************************************************************
    digitalWrite(LED, HIGH);
//****************************************************************


  EthernetClient client = server.available();
  if (client) {
    Serial.println("New Client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      char c = client.read();
      Serial.write(c);
      if (c == 'n' && currentLineIsBlank) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println("Refresh: 2");       //RECARREGA A PAGINA A CADA 2 SEGUNDOS
        client.println();
        client.println("<!DOCTYPE html>");
        client.println("<html><head>");
        //client.println("<meta http-equiv="content-type" content="text/html; charset=Windows-1252">");
        client.println("<title>");
        client.println("ESTAÇÃO METEOROLÓGICA");          //Titulo da Pagina (no navegador)
        client.println("</title>");
        client.println("</head>");
        client.println("<body>");
        client.println("<h1> ESTAÇÃO METEOROLÓGICA </h1>");     //Titulo da Pagina (na pagina)
        client.println("<p></p>");
        client.println("<table>");
        client.println("<tbody><tr>");
        client.println("<td>Sensor:</td>");                     //Nome dos Sensores
        client.println("<p></p>");
        client.println("<td>Status:</td>");                    //Status dos Sensores
        client.println("<td>Informação:</td>");               //Informação dos Sensores
        client.println("<td>Maxima:</td>");                  //Maxima Registrada Pelos Sensores
        client.println("<td>Mínima:</td>");                 //Minima Registrada Pelos Sensores
        client.println("</tr>");
        client.println("<tr>");
        client.println("<td>Temperatura: (<code>°</code>C)</td>");
        client.println("<td>Ligado</td>");
        client.println("<td>tempC</td>");           //Variavel do sensor
        client.println("<td>tempMax</td)");           //Variavel de Maxima Registrada do Sensor
        client.println("<td>tempMin</td>");          //Variavel de Minima Registrada do Sensor
        client.println("</tr>");
        client.println("<tr>");
        client.println("<td>Velocidade do Vento: (Km/h)</td>");
        client.println("<td>Ligado</td>");
        client.println("<td>windspeed</td>");         //Variavel do Sensor
        client.println("<td>maxspeed</td>");          //Variavel de Maxima Registrada do Sensor
        client.println("<td>  --  </td>");        //Variavel de Minima Registrada do Sensor
        client.println("</tr>");
        client.println("<tr>");
        client.println("<td>Umidade: (%)</td>");
        client.println("<td>Ligado</td>");
        client.println("<td>umidade</td>");       //Variavel do Sensor
        client.println("<td>umidademax</td>");       //Variavel de Maxima Registrada do Sensor
        client.println("<td>umidademin</td>");      //Variavel de Minima Registrada do Sensor
        client.println("</tr>");
        client.println("</tbody></table>");
        client.println("</body></html>");
        
      if (c == 'n') {
        currentLineIsBlank = true;
      }
      else if (c != 'r') {
        currentLineIsBlank = false;
      }
    }
  }
  delay (1);
  client.stop();
}

//****************************************************************
  windvelocity();
  RPMcalc();
//****************************************************************
  digitalWrite(LED, LOW);
//****************************************************************
//VELOCIDADE DO VENTO ATUAL
  SpeedWind();
  Serial.print(speedwind);
//****************************************************************
//MAIOR VELOCIDADE DO VENTO;
  Serial.print(maxspeed);
//****************************************************************
  digitalWrite(LED, HIGH);
//****************************************************************

//****************************************************************
  sensors.requestTemperatures();
  tempC = sensors.getTempC(sensor1);
  if(tempC>tempmax){
    tempmax=tempC;
  }else if(tempC<tempmin){
    tempmin=tempC;
  }
  Serial.print(tempC);
//****************************************************************

//****************************************************************
  digitalWrite(LED, LOW);
//****************************************************************

//UMIDADE*********************************************************
  calcumidade();
  Serial.print(umidade);

//****************************************************************
}

  void windvelocity(){
    speedwind = 0;
    
    counter = 0;  
    attachInterrupt(0, addcount, RISING);
    unsigned long millis();       
    long startTime = millis();
    while(millis() < startTime + period) {
    }
}
  
  
  
  void RPMcalc(){
    RPM=((counter)*60)/(period/1000);  //RPM
}
  
  
  
  void SpeedWind(){
    speedwind = (((4 * pi * radius * RPM)/60) / 1000)*3.6;  // VELOCIDADE DO VENTO EM KM/H
    
    if(speedwind>maxspeed)      //REGISTRA A MAIOR VELOCIDADE
      maxspeed=speedwind;
}
  
  
  void addcount(){
    counter++;
} 

  void calcumidade(){
    //CHAMA A LEITURA DA UMIDADE
    umidade = dht.readHumidity(); 

       if(umidade>umidademax){
        umidademax=umidade;

       }else if(umidade<umidademin){
        umidademin=umidade;   
       }
}
