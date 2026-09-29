#include <LCD16x2.h> //allows us to use the little LCD at the top
#include <Wire.h>

LCD16x2 lcd;

int intVal1 = 0, intVal2 = 0;
float floatVal1 = 0.0, floatVal2 = 0.0;


int E1 = 5;     //Port for M1 Speed Control
int E2 = 6;     //Port for M2 Speed Control

int M1 = 4;    //Port for M1 Direction Control
int M2 = 7;    //Port for M2 Direction Control

void GoForward(int speed)
{
  analogWrite (E1,speed); //Set M1 speed digitalWrite(M1,HIGH); //Set M1 direction 
  return; //end
}


void setup(){ //this code executes once at start
int b;

  Wire.begin();


pinMode(E1, OUTPUT); //These pins control the motor H-bridge circuitry, so we need to set them up as outputs
pinMode(E2, OUTPUT);
pinMode(M1, OUTPUT);
pinMode(M2, OUTPUT);
pinMode(A0, INPUT_PULLUP);
pinMode(A1, INPUT);



  lcd.lcdClear();
   
  lcd.lcdGoToXY(1,1);
  lcd.lcdWrite("Press white key");

  b = lcd.readButtons(); //Sample the state of the large white buttons

while(b == 15) // A value of 15 will only be seen if no buttons are pressed
{
  b = lcd.readButtons(); //Sample the state of the large white buttons
}

// Will only proceed as far as here once a button is pressed

// We want execution to get "trapped" in the above loop: otherwise, the programme would turn on the motors straigh away, and your robot might go sailing off the bench!
  
}

void loop(){ //this code loops continually after running through 'setup' once

  int displayed_voltage = 0;
  float actual_voltage, distance = 0;




  while(1){

    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    analogWrite(E1, 256);
    analogWrite(E2, 256);

    displayed_voltage = analogRead(A1);

    actual_voltage = map(displayed_voltage, 0, 500, 0, 5); //we found from testing the maximum voltage was around 500 volts
    distance = map(displayed_voltage, 0, 500, 30, 8); //from our testing the voltage approached a minimum at about 30 cm and max at about 8cm


    lcd.lcdClear();//eliminates all 'orphan' digits
    lcd.lcdGoToXY(1,1);
    lcd.lcdWrite(actual_voltage);
    lcd.lcdGoToXY(1,2);
    lcd.lcdWrite(distance); //prints the voltage and the distance on the lcd
    delay(1000); // gives time to read our findings

    
  }
}
