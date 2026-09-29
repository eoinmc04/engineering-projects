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

  int voltage_1, voltage_2, holes = 0; 
  float distance_travelled, revolutions, turns = 0;
  float gear_ratio = 5.0 /3.0;
  float circumfrence = 8.2; //declaration of all the variables

//while loop ensures counters and voltage reads don't reset between iterations
  while(1){

    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    analogWrite(E1, 256);
    analogWrite(E2, 256);

    voltage_1 = voltage_2;

    voltage_2 = analogRead(A0);

    /*lcd.lcdClear();
    lcd.lcdWrite(voltage_2); (used for testing the voltage)
    delay(1000);*/

    if(voltage_2 > 400 && voltage_1 < 400) //leading edge test compares consecutively sampled voltages and if they are quite spread apart, incriments the counter
    {
      holes++; //counting the holes increases the resolution by allowing us to sample 8 times per spin

      /*lcd.lcdClear();
      lcd.lcdWrite(holes); used for testing*/

      revolutions = float(holes) / 8.0; // 8 holes per gear so 1 revolution is 1/8 the number of holes
      
      lcd.lcdClear();
      lcd.lcdGoToXY(1,1);
      lcd.lcdWrite(revolutions); //prints the number of revolutions of our counter gear

      

      turns = gear_ratio * revolutions; //we used an extra gear to count the holes

      distance_travelled = turns * circumfrence; //amount the wheel has turned multiplied by

      lcd.lcdGoToXY(1,2);
      lcd.lcdWrite(distance_travelled); //prints the elapsed distance under the number of revolutions

    }
  }
}
