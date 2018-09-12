#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <Ethernet.h>
#include <SPI.h>


//ETHERNET************************
byte mac [ ] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };      //Define o MAC ADDRESS do Ethernet Shield
IPAddress ip(192,168,0,122);                               //Define IP
IPAddress gateway(192,168,0,1);                           //Define GATEWAY
IPAddress subnet(255, 255, 255, 0);                      //Define MASCARA DE REDE

EthernetServer server (81);                   //Inicia o SERVIDOR WEB na PORTA (81)

//********************************

//TERMOMETRO**********************
#define ONE_WIRE_BUS 5

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

void TEMPERATURA();
void UMIDADE();
void addcount();
void windvelocity();
void RPMcalc();
void SpeedWind();
void SpeedWind();
void ANEMOMETRO();

void setup() {
  Ethernet.begin(mac, ip, subnet, gateway);   
  server.begin();
  //Serial.begin(9600);
  sensors.begin();
  sensors.getDeviceCount();
  if(!sensors.getAddress(sensor1, 0))
    Serial.println("Sensor de Temperatura não encontrado!");
  dht.begin();
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);     //internall pull-up active
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);
  delay(200); 
  digitalWrite(LED,LOW);
  delay(200);
  digitalWrite(LED,HIGH);
  delay(200);
  digitalWrite(LED,LOW);
  delay(200);
  digitalWrite(LED,HIGH);
  delay(200);
  digitalWrite(LED,LOW);
  
}

void loop() {
  digitalWrite(LED, HIGH);
  TEMPERATURA();
  UMIDADE();
  ANEMOMETRO();
  digitalWrite(LED, LOW);
  
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
      while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        if(c == 'n' && currentLineIsBlank){
        // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 2"); //Recarrega a pagina a cada 2seg
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>");
          client.println("ESTA&Ccedil;&Atilde;O METEOROL&Oacute;GICA");          //Titulo da Pagina (no navegador)
          client.println("</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1><center><u> ESTA&Ccedil;&Atilde;O METEOROL&Oacute;GICA </u></center></h1>");     //Titulo da Pagina (na pagina)
          client.println("<center><table border=5 bgcolor=lightgray bordercolor = darkred cellspacing=10 cellpadding=3>");
          client.println("<tbody>");
          client.println("<tr>");
          client.println("<td align=center>Sensor:</td>");                     //Nome dos Sensores
          client.println("<p></p>");
          client.println("<td>Status:</td>");                    //Status dos Sensores
          client.println("<td align=center>Informa&ccedil;&atilde;o:</td>");               //Informação dos Sensores
          client.println("<td align=center>M&aacute;xima:</td>");                  //Maxima Registrada Pelos Sensores
          client.println("<td align=center>M&iacute;nima:</td>");                 //Minima Registrada Pelos Sensores
          client.println("</tr>");
          client.println("<tr>");
          client.println("<td align=center>Temperatura: (&ordm;C)</td>");
          client.println("<td align=center>");
          client.println("Ligado");
          client.println("</td>");
          client.println("<td align=center>");
          client.println(tempC);
          client.println("</td>");           //Variavel do sensor
          client.println("<td align=center>");
          client.println(tempMax);
          client.println("</td>");           //Variavel de Maxima Registrada do Sensor
          client.println("<td align=center>");
          client.println(tempMin);
          client.println("</td>");          //Variavel de Minima Registrada do Sensor
          client.println("</tr>");
          client.println("<tr>");
          client.println("<td align=center>Velocidade do Vento: (Km/h)</td>");
          client.println("<td align=center>");
          client.println("Ligado");
          client.println("</td>");
          client.println("<td align=center>");
          client.println(speedwind);
          client.println("</td>");         //Variavel do Sensor
          client.println("<td align=center>");
          client.println(speedwindmax);
          client.println("</td>");          //Variavel de Maxima Registrada do Sensor
          client.println("<td align=center>  --  </td>");        //Variavel de Minima Registrada do Sensor
          client.println("</tr>");
          client.println("<tr>");
          client.println("<td align=center>Umidade: (%)</td>");
          client.println("<td align=center>");
          client.println("Ligado");
          client.println("</td>");
          client.println("<td align=center>");
          client.println(h);
          client.println("</td>");       //Variavel do Sensor
          client.println("<td align=center>");
          client.println(hmax);
          client.println("</td>");       //Variavel de Maxima Registrada do Sensor
          client.println("<td align=center>");
          client.println(hmin);
          client.println("</td>");      //Variavel de Minima Registrada do Sensor
          client.println("</tr>");
          client.println("</tbody>");
          client.println("</table></center>");
          client.println("<p><center>");
          client.println("Esta&ccedil;&atilde;o localizada no Bairro Higien&oacute;polis - Porto Alegre RS");
          client.println("</center></p>");
          client.println("<p><center>");
          client.println("Mateus Scalco Rampon &and; Arthur Bockmann Grossi");
          client.println("</center></p>");
          client.println("</body>");
          client.println("</html>");
        break;
        }
        if (c == 'n') {
        // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != 'r') {
        // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    }
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
    
  }
  if(h > hmax){

    hmax = h;
  }

   /*Serial.print("Umidade : "); 
   Serial.print(h);
   Serial.println(" %");
   Serial.print("Umidade Maxima: "); 
   Serial.print(hmax);
   Serial.println(" %");
   Serial.print("Umidade Minima: "); 
   Serial.print(hmin);
   Serial.println(" %");*/

   //delay(2000);
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
  
  /*Serial.print("Velocidade do vento: ");
  Serial.print(speedwind);
  Serial.println(" [km/h] ");
  Serial.print("Velocidade do vento Maximo: ");
  Serial.print(speedwindmax);
  Serial.println(" [km/h] ");*/
  
  //delay(2000);                        //Delay entre as medições em milisegundos
}






