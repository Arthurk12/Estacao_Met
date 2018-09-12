#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Ethernet.h>


//ETHERNET************************
byte mac [ ] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,122);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255, 255, 255, 0);

EthernetServer server (81);
//********************************

//TERMOMETRO**********************
#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;

float tempMin=999;
float tempMax=0;
float tempC=0;
//*******************************

//DHT21**************************
#define DHTPIN 6
#define DHTTYPE DHT21

DHT dht(DHTPIN, DHTTYPE);

float hmin=999;
float hmax=0;
float h=0;
//*******************************

//ANEMOMETRO*********************
# define Hall sensor 2
// Constants definitions
#define pi 3.14159265
#define period 5000
#define radius 147

// Variable definitions
unsigned short int counter = 0;
unsigned short int RPM = 0;
float speedwind = 0;
float speedwindmax=0;
float medspeed=0;
//*******************************

//LED****************************
#define LED 9
//*******************************


void setup() {
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  sensors.begin();
  sensors.getAddress(sensor1, 0);
  dht.begin();
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  Serial.begin(9600);
  pinMode(9,OUTPUT);
  
}

void TEMPERATURA(){
  
  
  //Le informaçoes do sensor
  sensors.requestTemperatures();
  tempC = sensors.getTempC(sensor1);
  // Atualiza temperaturas minima e maxima
  if (tempC < tempMin){
    tempMin = tempC;
  }
  if (tempC > tempMax){
    tempMax = tempC;
  }

}

void UMIDADE(){
  h = dht.readHumidity();


  if(h < hmin){
    
    hmin = h;
    
  }else if(h > hmax){

    hmax = h;
  }
}

void addcount(){
  counter++;
} 

void windvelocity(){
  
  counter = 0;  
  attachInterrupt(0, addcount, RISING);
  unsigned long millis();       
  long startTime = millis();
  while(millis() < startTime + period) {
  }
}

void RPMcalc(){
  RPM=((counter)*60)/(period/1000);
}

void SpeedWind(){
  speedwind = (((4 * pi * radius * RPM)/60) / 1000)*3.6;
}

void ANEMOMETRO(){
  windvelocity();
  RPMcalc();
  //print km/h  
  SpeedWind();

  if(speedwind > speedwindmax)
    speedwindmax = speedwind;
}

void MEDIAVEL(){

  if(medspeed==0){
    medspeed=speedwind;
  }else{
    medspeed+=speedwind;
    medspeed=medspeed/2;
  }
}

void loop() {
  digitalWrite(LED, HIGH);
  TEMPERATURA();
  UMIDADE();
  digitalWrite(LED, LOW);
  ANEMOMETRO();
  MEDIAVEL();
  digitalWrite(LED, HIGH);
  
  EthernetClient client = server.available();
  if (client) {
    Serial.println("New Client");
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      char c = client.read();
      Serial.write(c);
      if (c == 'n' && currentLineIsBlank){
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println("Refresh: 2");
        client.println();
        client.println("<!DOCTYPE html>");
        client.println("<html><head>");
        client.println("<title>");
        client.println("ESTA&Ccedil;&Atilde;O METEOROL&Oacute;GICA"); 
        client.println("</title>");
        client.println("</head><body>");
        client.println("<center><h1><b><u> ESTA&Ccedil;&Atilde;O METEOROL&Oacute;GICA </u></b></h1></center>");
        client.println("<p></p>");
        client.println("<table width=20% border=40 bordercolor = #4169E1 bgcolor = #DCDCDC cellspacing=20 align=center><tbody>");
        client.println("<tr>");
        client.println("<td><center><b>Sensor:</b></td></center>");
        client.println("<td><center><b>Status:</b></td></center>");
        client.println("<td><center><b>Atual:</b></td></center>");
        client.println("<td><center><b>M&aacute;xima:</b></td></center>");
        client.println("<td><center><b>M&iacute;nima:</b></td></center>");
        client.println("<td><center><b>M&eacute;dia:</b></td></center>");
        client.println("</tr>");
        client.println("<tr>");
        client.println("<td><center><b>Temperatura:(&#176;C)</b></td></center>");
        client.println("<td><center>Ligado</td></center>");
        client.println("<td><center>");
        client.println(tempC);
        client.println("</td></center>");
        client.println("<td><center>");
        client.println(tempMax);
        client.println("</td></center>");
        client.println("<td><center>");
        client.println(tempMin);
        client.println("</td></center>");
        client.println("<td><center>");
        client.println("--------");
        client.println("</td></center>");
        client.println("</tr>");
        client.println("<tr>");
        client.println("<td><center><b>Velocidade do Vento: (Km/h)</b></td></center>");
        client.println("<td><center>Ligado</td></center>");
        client.println("<td><center>");
        client.println(speedwind);
        client.println("</td></center>");
        client.println("<td><center>");
        client.println(speedwindmax);
        client.println("</td></center>");
        client.println("<td><center>");
        client.println("--------");
        client.println("</td></center>");
        client.println("<td><center>");
        client.println(medspeed);
        client.println("</td></center>");
        client.println("</tr>");
        client.println("<tr>");
        client.println("<td><center><b>Umidade: (%)</b></td></center>");
        client.println("<td><center>Ligado</td></center>");
        client.println("<td><center>");   
        client.println(h);
        client.println("</td></center>");
        client.println("<td><center>");   
        client.println(hmax);
        client.println("</td></center>");
        client.println("<td><center>");   
        client.println(hmin);
        client.println("</td></center>");
        client.println("<td><center>");
        client.println("--------");
        client.println("</td></center>");
        client.println("</tr></tbody></table></body>");
        client.println("<h5><center>Todos Direitos Reservados Copyright &copy; Arthur B&ouml;ckmann Grossi &and; Mateus Scalco Rampon</center></h5>");
        client.println("</html>");
        break;

      }
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
  digitalWrite(LED, LOW);
}




