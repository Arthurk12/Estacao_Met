#include <math.h>

#define WindSensorPin (2) // The pin location of the anemometer sensor D2

  volatile unsigned long Rotations; // Fan rotation counter used in interrupt routine
  
  volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine
  
  float WindSpeed; // Speed m/s

void setup() {

  Serial.begin(9600);
  
  pinMode(WindSensorPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(WindSensorPin), isr_rotation, RISING);
  
  Serial.println("Wind Speed Test");
  
  Serial.println("Rotations\tM/S");

}

void loop() {

  Rotations = 0; // Set Rotations count to 0 ready for calculations
  
  //sei(); // Enables interrupts
  
  delay (3000); // Wait 3 seconds to average
  
  //cli(); // Disable interrupts
  
  // Convert to m/s using the derived formula (see notes)
  
  // V = N(0.35/3) = N * 0.12

  // 360 GRAUS = 6,2832 RADS
  // RADS/S = ANGULO/TEMPO = 6,2832/3
  // V = (6,2832/3) * raio(metros) = 2,0944 * 0.147
  
  WindSpeed = Rotations * 0.3078768;
  
  Serial.print(Rotations); Serial.print("\t\t");
  
  Serial.println(WindSpeed*3,6);

}

// This is the function that the interrupt calls to increment the rotation count

void isr_rotation () {
  
  if ((millis() - ContactBounceTime) > 15 ) { // Debounce the switch contact
  
    Rotations++;
    
    ContactBounceTime = millis();
    

  }
  //Serial.print("ISR");

}
