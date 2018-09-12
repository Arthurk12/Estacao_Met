#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Ethernet.h>


//ETHERNET************************
byte mac [ ] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };      //Define o MAC ADDRESS do Ethernet Shield
IPAddress ip(192,168,0,122);                               //Define IP
IPAddress gateway(192,168,0,1);                           //Define GATEWAY
IPAddress subnet(255, 255, 255, 0);                      //Define MASCARA DE REDE

EthernetServer server (81);                   //Inicia o SERVIDOR WEB na PORTA (81)
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
#define DHTPIN 6                                    // Pino conectado ao pino de dados do sensor
#define DHTTYPE DHT21                              // Sensor DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);                         // Definicoes do sensor : pino, tipo

float hmin=999;
float hmax=0;
float h=0;
//*******************************

//ANEMOMETRO*********************
# define Hall sensor 2         //Define o sensor no Pino 2
// Constants definitions
const float pi = 3.14159265;       // Numero pi
#define period 5000              // Tempo de medida(milisegundos)
#define radius 147              // Raio do anemometro(mm)

// Variable definitions
unsigned int counter = 0;     // Contador magnetico para o sensor
unsigned int RPM = 0;        // Rotações por minuto
float speedwind = 0;       // Velocidade do vento (km/h)
float speedwindmax=0;
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
  digitalWrite(2, HIGH);     //internall pull-up active
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
  /*Serial.print("TEMP:");
  Serial.println(tempC);
  Serial.print("TEMPMAX:");
  Serial.println(tempMax);
  Serial.print("TEMPMIN:");
  Serial.println(tempMin);*/

}

void UMIDADE(){
  h = dht.readHumidity();                 // Leitura da umidade


  if(h < hmin){
    
    hmin = h;
    
  }else if(h > hmax){

    hmax = h;
  }
  
   /*Serial.print("Umidade : "); 
   Serial.print(h);
   Serial.println(" %");
   Serial.print(hmax);
   Serial.println(" %");
   Serial.print(hmin);
   Serial.println(" %");*/
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
  RPM=((counter)*60)/(period/1000);                       // Calculo de Rotações por minuto(RPM)
}

void SpeedWind(){
  speedwind = (((4 * pi * radius * RPM)/60) / 1000)*3.6;  // Calcula a velocidade do vento em km/h
}

void ANEMOMETRO(){
  windvelocity();
  RPMcalc();
  //print km/h  
  SpeedWind();

  if(speedwind > speedwindmax)
    speedwindmax = speedwind;

  /*Serial.print(speedwind);
  Serial.println(" [km/h] ");
  Serial.print(speedwindmax);
  Serial.println(" [km/h] ");*/
  
  //delay(2000);                        //Delay entre as medições em milisegundos
}

void loop() {
  digitalWrite(LED, HIGH);
  TEMPERATURA();
  UMIDADE();
  ANEMOMETRO();
  digitalWrite(LED, LOW);
  
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
        client.println("Refresh: 2");       //RECARREGA A PAGINA A CADA 2 SEGUNDOS
        client.println();
        client.println("<!DOCTYPE html>");
        client.println("<html><head>");
        client.println("<title>");
        client.println("ESTA&Ccedil;&Atilde;O METEOROL&Oacute;GICA");          //Titulo da Pagina (no navegador)
        client.println("</title>");
        client.println("</head><body>");
        client.println("<center><h1><b><u> ESTA&Ccedil;&Atilde;O METEOROL&Oacute;GICA </u></b></h1></center>");     //Titulo da Pagina (na pagina)
        client.println("<p></p>");
        client.println("<table width=20% border=40 bordercolor = #4169E1 bgcolor = #DCDCDC cellspacing=20 align=center><tbody>");
        client.println("<tr>");
        client.println("<td><center><b>Sensor:</b></td></center>");                     //Nome dos Sensores
        client.println("<td><center><b>Status:</b></td></center>");                    //Status dos Sensores
        client.println("<td><center><b>Atual:</b></td></center>");               //Informação dos Sensores
        client.println("<td><center><b>M&aacute;xima:</b></td></center>");                  //Maxima Registrada Pelos Sensores
        client.println("<td><center><b>M&iacute;nima:</b></td></center>");                 //Minima Registrada Pelos Sensores
        client.println("</tr>");
        client.println("<tr>");
        client.println("<td><center><b>Temperatura:(&#176;C)</b></td></center>");
        client.println("<td><center>Ligado</td></center>");
        client.println("<td><center>");
        client.println(tempC);           //Variavel do sensor
        client.println("</td></center>");
        client.println("<td><center>");
        client.println(tempMax);           //Variavel de Maxima Registrada do Sensor
        client.println("</td></center>");
        client.println("<td><center>");
        client.println(tempMin);           //Variavel de Minima Registrada do Sensor
        client.println("</td></center>");
        client.println("</tr>");
        client.println("<tr>");
        client.println("<td><center><b>Velocidade do Vento: (Km/h)</b></td></center>");
        client.println("<td><center>Ligado</td></center>");
        client.println("<td><center>");
        client.println(speedwind);   //Variavel do Sensor
        client.println("</td></center>");
        client.println("<td><center>");
        client.println(speedwindmax);   //Variavel de Maxima Registrada do Sensor
        client.println("</td></center>");
        client.println("<td><center>");
        client.println("--------");
        client.println("</td></center>");
        client.println("</tr>");
        client.println("<tr>");
        client.println("<td><center><b>Umidade: (%)</b></td></center>");
        client.println("<td><center>Ligado</td></center>");
        client.println("<td><center>");   
        client.println(h);         //Variavel do Sensor
        client.println("</td></center>");
        client.println("<td><center>");   
        client.println(hmax);         //Variavel de Maxima Registrada do Sensor
        client.println("</td></center>");
        client.println("<td><center>");   
        client.println(hmin);         //Variavel de Minima Registrada do Sensor
        client.println("</td></center>");
        client.println("</tr></tbody></table></body>");
        client.println("<h5><center>Todos Direitos Reservados Copyright &copy; Arthur B&ouml;ckmann &and; Mateus Rampon Scalco</center></h5>");
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
}




