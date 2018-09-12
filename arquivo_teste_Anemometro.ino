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



void setup(){
  // Set the pins
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);     //internall pull-up active
    
  //Start serial 
  Serial.begin(9600);       // Configura a porta serial para 9600 baud
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
  Serial.print(speedwind);
  Serial.print(" [km/h] ");  
  Serial.println();
  
  delay(2000);                        //Delay entre as medições em milisegundos
}

void loop(){
  ANEMOMETRO();
}



