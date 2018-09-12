// Programa : Sensor de temperatura DS18B20
// Autor : FILIPEFLOP

#include <OneWire.h>
#include <DallasTemperature.h>

// Porta do pino de sinal do DS18B20
#define ONE_WIRE_BUS 5

// Define uma instancia do oneWire para comunicacao com o sensor
OneWire oneWire(ONE_WIRE_BUS);

// Armazena temperaturas minima e maxima
float tempMin = 999;
float tempMax = 0;

DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;

void mostra_endereco_sensor(DeviceAddress deviceAddress);

void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
  // Localiza e mostra enderecos dos sensores
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
  if (!sensors.getAddress(sensor1, 0)) 
    Serial.println("Sensores nao encontrados !"); 
  // Mostra o endereco do sensor encontrado no barramento
  Serial.print("Endereco sensor: ");
  mostra_endereco_sensor(sensor1);
  Serial.println();
  Serial.println();
}

void mostra_endereco_sensor(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}
void loop()
{
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(sensor1);
  if (tempC < tempMin)
  {
    tempMin = tempC;
  }
  if (tempC > tempMax)
  {
    tempMax = tempC;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Min : ");
  Serial.print(tempMin);
  Serial.print(" Max : ");
  Serial.println(tempMax);
}
