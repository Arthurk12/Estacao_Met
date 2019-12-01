void setup() {
  Serial.begin(9600);
  

}

void loop() {
  float Tensao;
  String DirecaoVento;
  int PINO_BIR = A0;
  
  Tensao = analogRead(PINO_BIR)*(5.0/1023.0);
//  if (Tensao <= 0.28){
//    DirecaoVento = "Noroeste (NO)";   //NO
//  }else if (Tensao <= 0.30){
//    DirecaoVento = "Oeste (O)";       //O
//  }else if (Tensao <= 0.36){
//    DirecaoVento = "Sudoeste (SO)";   //SO
//  }else if (Tensao <= 0.43){
//    DirecaoVento = "Sul (S)";         //S
//  }else if (Tensao <= 0.51){
//    DirecaoVento = "Sudeste (SE)";    //SE
//  }else if (Tensao <= 0.68){
//    DirecaoVento = "Leste (E)";       //E
//  }else if (Tensao <= 0.94){
//    DirecaoVento = "Nordeste (NE)";   //NE
//  }else{
//    DirecaoVento = "Norte (N)";       //N
//  };

  Serial.print("Tensao: ");
  Serial.println(Tensao);

  delay(1000);

}
