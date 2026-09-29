#include <LCD16x2.h> //allows us to use the little LCD at the top
#include <Wire.h>
#include <Servo.h>

Servo myservo_grey;   // create servo object to control a servo https://shop.pimoroni.com/products/geekservo-270-lego-compatible-servo
Servo myservo_green;  // create servo object to control a servo https://shop.pimoroni.com/products/geekservo-lego-compatible-continuous-rotation-servo

LCD16x2 lcd;

int intVal1 = 0, intVal2 = 0;
float floatVal1 = 0.0, floatVal2 = 0.0;


int E1 = 5;     //Port for M1 Speed Control
int E2 = 6;     //Port for M2 Speed Control

int M1 = 4;    //Port for M1 Direction Control
int M2 = 7;    //Port for M2 Direction Control


void setup(){ //this code executes once at start
int b;

  Wire.begin();


pinMode(E1, OUTPUT); //These pins control the motor H-bridge circuitry, so we need to set them up as outputs
pinMode(E2, OUTPUT);
pinMode(M1, OUTPUT);
pinMode(M2, OUTPUT);


  myservo_green.attach(10);  // attaches the servo on pin D10 to the servo object

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

  lcd.lcdClear();
  
}

void loop(){ //this code loops continually after running through 'setup' once

static int cur_speed = 100;
static int mapped_speed;

drive(255);

cur_speed = constrain(cur_speed, -100, 100); //Keep it between -100 and +100, e.g max speed clockwise and counter-clockwise
mapped_speed = map(cur_speed, -100, 100, 0, 180); //Arduino servo library expect servos with a 180 degree range. 


myservo_green.write(mapped_speed);                // Argument sets the speed of the green servo rotation
                                    

}
void drive(int speed){

  analogWrite(E1, speed);
  digitalWrite(M1, HIGH);
  analogWrite(E2, speed);
  digitalWrite(M2, LOW);

}