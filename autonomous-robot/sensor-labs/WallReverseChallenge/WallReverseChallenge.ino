/*Report: This is team 6's report, (Abigail, Eoin and Paddy). The goal of this programme was to be able to make 
our robot reverse for 20cm when it came within 10cm of a wall. Firstly we set up the optical sensor in such a way that it
could count the number of holes on a gear(voltage high when pointing through hole and low when not). We recorded the number 
of rising edges by sampling voltages and then moving the stale sample into our previous voltage variable putting the newly 
recorded voltage into a second variable. We then used the fact that there are 8 holes on the gear to count its number of revolutions.
for every 3 revolution of this gear the wheel would turn 5 times. We used this information and multiplied it by the circumfrence of
the wheel to record the distance in a variable so we could keep track of it. To calibrate the distance from the distance sensor,
we tested the voltages that would be given at certain distances. We used this and the mapping function to approximately record the
distance of a wall away from this sensor. All of this work is contained in our sketches for task 1 and task 2. The following
programme is the programme we created for the challenge. We did not get to test this programme as we were working on the chasis of the
robot and didn't get time to put it back together to run and test this programme fully, so we can't be sure whether it works or not, however
we belive there is a good chance that this would work (with a few minor tweaks) in a fully completed robot based on the tests other tests
we performed and that the programmes written for those tasks were successful*/


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
pinMode(A1, INPUT);//declare input pins for both sensors


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
  int displayed_voltage = 0;
  float actual_voltage, distance = 0;

  displayed_voltage = analogRead(A1);
  distance = map(displayed_voltage, 0, 500, 30, 8); //from our testing the voltage approached a minimum at about 30 cm and max at about 8cm
  
  
  //This while loop keeps the robot driving straight while the distance read on the sensor is greater than 10
  while(distance > 10){

    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    analogWrite(E1, 100);
    analogWrite(E2, 100); // distance set to 100 slows the robot down

    voltage_1 = voltage_2;

    voltage_2 = analogRead(A0);

    /*lcd.lcdClear();
    lcd.lcdWrite(voltage_2); (used for testing the voltage)
    delay(1000);*/

    if(voltage_2 > 400 && voltage_1 < 400)
    {
      holes++;

      /*lcd.lcdClear();
      lcd.lcdWrite(holes); used for testing*/

      revolutions = float(holes) / 8.0; // 8 holes per gear so 1 revolution is 18th the number of holes

      turns = gear_ratio * revolutions; //we used an extra gear to count the holes

      distance_travelled = turns * circumfrence; //amount the wheel has turned multiplied by

      lcd.lcdGoToXY(1,1);
      lcd.lcdWrite(distance_travelled); //prints the elapsed distance under the number of revolutions
    }


  }
    //stops the robot for 1 second
    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    analogWrite(E1, 0);
    analogWrite(E2, 0);
    delay(1000);

    distance_travelled = 0;
    //following while loop should stop afrter robot has reversed 20 cm
    while(distance_travelled < 20){
    //get the robot to reversing start 
    digitalWrite(M1, LOW);
    digitalWrite(M2, HIGH);
    analogWrite(E1, 100);
    analogWrite(E2, 100);

    voltage_1 = voltage_2; // transferred stale voltage into voltage 1 leaving voltage two able to take another value

    voltage_2 = analogRead(A0);


    if(voltage_2 > 400 && voltage_1 < 400) //threshold for the rising edge of the voltage
    {
      holes++;

      revolutions = float(holes) / 8.0; // 8 holes per gear so 1 revolution is 1/8 the number of holes

      turns = gear_ratio * revolutions; //we used an extra gear to count the holes

      distance_travelled = turns * circumfrence; //amount the wheel has turned multiplied by

      lcd.lcdGoToXY(1,1);
      lcd.lcdWrite(distance_travelled); //prints the elapsed distance under the number of revolutions
    }


    }

  //Get robot to stop for a long period

    digitalWrite(M1, HIGH);
    digitalWrite(M2, LOW);
    analogWrite(E1, 0);
    analogWrite(E2, 0);
    delay(100000);

}