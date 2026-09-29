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


  myservo_grey.attach(9);    // attaches the servo on pin D9 to the servo object
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

  lcd.lcdGoToXY(1,1);
  lcd.lcdWrite("^ Spd v");

  lcd.lcdGoToXY(10,1);
  lcd.lcdWrite("^ Ang v");
  
}

void loop(){ //this code loops continually after running through 'setup' once
static int b;
static int cur_ang = 135;
static int mapped_angle;
static int cur_speed = 0;
static int mapped_speed;

  b = lcd.readButtons(); //Sample the state of the large white buttons

if(~b & 0b1000) //Note the use of bit-masking to select a particular large white button
{
cur_ang = cur_ang - 4;
}

if(~b & 0b0100)
{
cur_ang = cur_ang + 4;
}

if(~b & 0b0010)
{
cur_speed = cur_speed - 5;
}

if(~b & 0b0001)
{
cur_speed = cur_speed + 5;
}


cur_ang = constrain(cur_ang, 0, 270); //Keep it between 0 and 270 degrees
mapped_angle = map(cur_ang, 0, 270, 0, 180); //Arduino servo library expect servos with a 180 degree range, whereas ours can manage 270

cur_speed = constrain(cur_speed, -100, 100); //Keep it between -100 and +100, e.g max speed clockwise and counter-clockwise
mapped_speed = map(cur_speed, -100, 100, 0, 180); //Arduino servo library expect servos with a 180 degree range. 

  lcd.lcdGoToXY(12,2);
  lcd.lcdWrite("      ");
  lcd.lcdGoToXY(12,2);
  lcd.lcdWrite(cur_ang);

  lcd.lcdGoToXY(3,2);
  lcd.lcdWrite("      ");
  lcd.lcdGoToXY(3,2);
  lcd.lcdWrite(cur_speed);

  myservo_grey.write(mapped_angle);    // Argument sets the angle of the grey servo
 myservo_green.write(mapped_speed);                // Argument sets the speed of the green servo rotation
           
  delay(25);                         

}
