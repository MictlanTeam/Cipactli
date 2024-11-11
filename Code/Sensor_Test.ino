const int ledPIN = PC13;
const int ledPCB = PC14;
HardwareTimer pwmtimer3(4);
HardwareTimer pwmtimer4(4);
HardwareTimer pwmtimer1(1);
int an0, an1, an2, an3, an4, an5, an6, an7, an8, an9;
int i = 0;
bool z = 0;
int b,c;
void setup() {
  
  pwmtimer3.pause();
  pwmtimer3.setOverflow(3600);  
  pwmtimer3.refresh();                          
  pwmtimer3.resume();                           
                                                
  pwmtimer4.pause();
  pwmtimer4.setOverflow(3600);  //igual para este canal
  pwmtimer4.refresh();
  pwmtimer4.resume();

  pwmtimer1.pause();
  pwmtimer1.setPrescaleFactor(32); 
  pwmtimer1.setOverflow(45000-1);  
  
  pwmtimer1.refresh();
  pwmtimer1.resume();

  pinMode(PC13, OUTPUT);  //LED bluepill
  pinMode(PC14, OUTPUT);  //LED Placa
  pinMode(PC15, OUTPUT);  //LED
  pinMode(PB6, OUTPUT);   //DIR PWM2//pin de direccion
  pinMode(PB7, OUTPUT);   //DIR PWM1//pin de direccion

  pinMode(PB8, PWM); //PWM2
  pinMode(PB9,PWM); //PEM1
  pinMode(PA8,PWM); //PWM Turbina

  
  pwmWrite(PB8, 0);  //PWM2 T4C2
  pwmWrite(PB9, 0);  //PWM1 T4C1

  digitalWrite(PC13, HIGH);
  digitalWrite(PB6, HIGH);
  digitalWrite(PB7, HIGH);
  pwmWrite(PA8, 1500);
  Serial.begin(9600);
}

void loop() {

an0=analogRead(PA0);
an1=analogRead(PA1);
an2=analogRead(PA2);
an3=analogRead(PA3);
an4=analogRead(PA4);
an5=analogRead(PA5);
an6=analogRead(PA6);
an7=analogRead(PA7);
an8=analogRead(PB0);
an9=analogRead(PB1);
  
 
  digitalWrite(ledPIN, HIGH);
  digitalWrite(ledPCB, HIGH);
  delay(400);
  Serial.print("0:  ");
  Serial.println(an0);
  Serial.print("1:  ");
  Serial.println(an1);
  Serial.print("2:  ");
  Serial.println(an2);
  Serial.print("3:  ");
  Serial.println(an3);
  Serial.print("4:  ");
  Serial.println(an4);
  Serial.print("5:  ");
  Serial.println(an5);
  Serial.print("6:  ");
  Serial.println(an6);
  Serial.print("7:  ");
  Serial.println(an7);
  Serial.print("8:  ");
  Serial.println(an8);
  Serial.print("9:  ");
  Serial.println(an9);
  Serial.println();
  Serial.println();
  digitalWrite(ledPIN, LOW);
  digitalWrite(ledPCB, LOW);
  delay(400);
  
}
