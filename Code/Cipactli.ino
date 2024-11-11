

/*PID 
CONTROL PID
SEP 2023
*/

///---Define's---///
#define MAX_SPEED 3200                                               //MAXIMA VELOCIDAD DEL ROBOT, CUANDO TIENE QUE AJUSTAR (CURVAS) valor entre 0 y 3600
#define MED_SPEED 3000                                              //VELOCIDAD EN RECTAS (valor entre 0 y 3600)
#define OUT_SPEED 3000                                              //VELOCIDAD DE AJUSTE CUANDO PIERDE EN SU TOTALIDAD LA LINEA (valor entre 0 y 3600)
#define OUT_LINE   16                                              //valor dado para fuera de linea NO CAMBIAR
#define KP         160                                              //CONSTANTE P DEL CONTROL PID
#define KD         250                                              //CONSTANTE D DEL CONTROL PID
#define KI          10                                              //CONSTANTE I DEL CONTROL PID
#define valturbina 4200                                            //Velocidad de la turbina RANGO  de 1500 a 5250
enum {HOME,STARTING,RUN}state = HOME;//se enumeran home, starting y run con 0,1 y2. state 
enum {CENTER,RIGHT,LEFT} out_state = CENTER;
int sensor[10];

const int ledPIN = PC13;
const int ledPCB = PC14;
HardwareTimer pwmtimer3(4);
HardwareTimer pwmtimer4(4);
HardwareTimer pwmtimer1(1);

int i = 0;
bool z = 0;
int b,c;

int pin2 = 0,pin3=0,pin4=0,pin5=0,pin6=0,pin7=0,pin8=0,pin9=0,pin10=0,pin11=0,pin12=0,pin14=0,pin15=0,pin16=0,pin17=0,pin18=0,pin19=0;
   //Local variables//
   signed int error_actual = 0;
    signed int error_anterior = 0;
    int error_sum = 0;
    int errores=0;
    int errorx=0;
   
   signed int speed_1  =0; //Motor 2 - left side
    signed int speed_2  =0; //Motor 1 - right side
   
   signed int proporcional = 0;
    signed int derivativo = 0;
    signed int integrativo = 0;

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
  pinMode(PB6, OUTPUT);   //DIR PWM2//pin de direccion
  pinMode(PB7, OUTPUT);   //DIR PWM1//pin de direccion
  pinMode(PB5, INPUT);    //Boton
  pinMode(PB3, INPUT);    //GO
  pinMode(PB4, INPUT);    //Ready
  pinMode(PC15, INPUT);  //LED

  pinMode(PB8, PWM); //PWM2
  pinMode(PB9,PWM); //PEM1
  pinMode(PA8,PWM); //PWM Turbina

  
  pwmWrite(PB8, 0);  //PWM2 T4C2
  pwmWrite(PB9, 0);  //PWM1 T4C1

  digitalWrite(ledPIN, LOW);
  digitalWrite(PB6, LOW); //Atras
  digitalWrite(PB7, LOW); //Atras
 

  pwmWrite(PA8, 1500); //Turbina velocidad en 0
  
}

///---Functions---///
///Function to read error///
signed int Read_error (void)
{
    
signed int error = 0;
int i,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,umbral=3000;
sensor[0]=analogRead(PA0);
sensor[1]=analogRead(PA1);
sensor[2]=analogRead(PA2);
sensor[3]=analogRead(PA3);
sensor[4]=analogRead(PA4);
sensor[5]=analogRead(PA5);
sensor[6]=analogRead(PA6);
sensor[7]=analogRead(PA7);
sensor[8]=analogRead(PB0);
sensor[9]=analogRead(PB1);

   for(i=0;i<10;i++)
   {
    if(sensor[i]>umbral)
    {
      sensor[i]=0;
    }
    else
    {
      sensor[i]=1;
    }
   }

   b0=!(sensor[0]);
   b1=!(sensor[1]);
   b2=!(sensor[2]);
   b3=!(sensor[3]);
   b4=!(sensor[4]);
   b5=!(sensor[5]);
   b6=!(sensor[6]);
   b7=!(sensor[7]);
   b8=!(sensor[8]);
   b9=!(sensor[9]);


   if (b0||b1||b2||b3||b4||b5||b6||b7||b8||b9) {  //Out line - Black line (si alguno lee linea entra)
   
      /*Negative left sensor*/
        error = (b4)         ? (0-2) : error; //(0-2) operacion para que el comilador vea un -2
        error = (b3)         ? (0-4) : error;
        error = (b2)         ? (0-6) : error;
        error = (b1)         ? (0-8) : error;
        error = (b0)         ? (0-10) : error;
        error = (b4 && b3)   ? (0-3) : error; //se leen sensores dobles al final
        error = (b3 && b2)   ? (0-5) : error;
        error = (b2 && b1)   ? (0-7) : error;
        error = (b1 && b0)   ? (0-9) : error;
             
        
        /*Positive right sensor*/
        error = (b5)         ? 2 : error;
        error = (b6)         ? 4 : error;
        error = (b7)         ? 6 : error;
        error = (b8)         ? 8 : error;
        error = (b9)        ? 10 : error;
        error = (b5 && b6)   ? 3 : error;
        error = (b6 && b7)   ? 5 : error;
        error = (b7 && b8)   ? 7 : error;
        error = (b8 && b9)   ? 9 : error;
        
             
        /*Neutral middle sensor*/
       
        error = (b4 && b5)   ? 0 : error;
        
        out_state = ((error <= 4)&&(error >=(0-4)))  ? CENTER : out_state; //fuera de linea
        out_state = ((error >= 5)&&(error <=9))        ? LEFT : out_state;
        out_state = ((error <=(0-5))&&(error >=(0-9))) ? RIGHT: out_state;

        return error;
    }
      else {
    return OUT_LINE;
    }
}

///Function to set speed of right motor///
void Motor_R (signed int speed)
{
   if(!speed) {

   pwmWrite(PB8, 0);// M1 Duty 0% 
    
   
   }
   else {
     
      speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;

      if (speed >=1){
      pwmWrite(PB8,speed);  
      digitalWrite(PB6, HIGH);
            
      
      }
      else {
      speed *= (0-1);
      ////////////////////////
      speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;
      ///////////////////////////////////
      pwmWrite(PB8,speed); 
      digitalWrite(PB6, LOW); //Cambio de giro
      
      }
   } 
   return;
}

///Function to set speed of left motor///
void Motor_L (signed int speed)
{  
   if(!speed) {
   pwmWrite(PB9, 0);  
   
   }
   else {
     
    
      speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;

      if (speed >=1){
      pwmWrite(PB9,speed);  
      digitalWrite(PB7, HIGH); //high adelante M1
      }
      else {
      speed *= (0-1);
      ////////////////////////
      speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;
      ///////////////////////////////////
      pwmWrite(PB9,speed);
      digitalWrite(PB7, LOW); 
      }
   }
   return;
}

void loop() {
  switch (state)
      {
         case HOME:
            Motor_R(0);
            Motor_L(0);
            
            if(digitalRead(PB4) == 1){
               state = STARTING;
               pwmWrite(PA8, valturbina); //Velocidad de la turbina RANGO  de 1500 a 5250
            }
            digitalWrite(PC14, HIGH);
            delay(500);
            digitalWrite(PC14, LOW);
            delay(500);
            digitalWrite(PC14, HIGH);
            break;
         
         case STARTING:
            digitalWrite(PC14, LOW);
            if(digitalRead(PC15) == 1){
               state = RUN;
               digitalWrite(PC14, HIGH);
            }
            break;
            
         case RUN:
            //Read real error -10 to 10
            error_actual = Read_error();
            errorx=error_actual;
            //Out line
            if(error_actual == OUT_LINE) {
               //Out line state machine//
               switch (out_state)
                    {
                        case CENTER:
                            speed_1 = MED_SPEED;
                            speed_2 = MED_SPEED;
                            
                            break;
                        case LEFT:
                            speed_1 = MAX_SPEED;
                            speed_2 = (0-OUT_SPEED);
                            
                            break;
                        case RIGHT:
                            speed_1 = (0-OUT_SPEED);
                            speed_2 = MAX_SPEED;
                            
                            break;
                    }
            }
            //On line
            else {
                    integrativo = (KI * errores);
                    proporcional = (KP * error_actual);
                    derivativo = (KD * (error_actual - error_anterior));
               
               error_sum ++;
                    if(error_sum > 300) {// valor que se puede modificar para la acomulaicon de errores para el kd
                       error_anterior = error_actual;
                       error_sum = 0;
                       errores += error_actual;
                       errores=(errores>=3000)?3000:errores;
                    }
                    if(error_actual == 0) {
                       errores = 0;
                    }
                  
                    speed_1  = MED_SPEED + (proporcional + derivativo + integrativo);
                    speed_2  = MED_SPEED - (proporcional + derivativo + integrativo);
            }
            
            Motor_R(speed_2);
            Motor_L(speed_1);

            if(digitalRead(PC15) == 0){//se REINICIA SI SE PRESIONA EL BOTON
              Motor_R(-OUT_SPEED);
              Motor_L(-OUT_SPEED);
              delay(100);
              pwmWrite(PA8, 1500);// Valor turbina, apagada.

               state = HOME;
               
               
            }
            break;
      }
   
}
