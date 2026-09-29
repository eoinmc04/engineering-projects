#include <LCD16x2.h> //allows us to use the little LCD shield at the top
#include <Wire.h> //allows us to communicate with the LCD shield using the I2C protocol
#include <Servo.h>
#include <Arduino_NineAxesMotion.h>
// To allow this code to work, you will need to add the 'EnableInterrupt' library. You can do this easily using the menus Sketch -> Include Library -> Manage Libraries...
#include <EnableInterrupt.h> //allows us to use interrupts on any pin

NineAxesMotion mySensor;

Servo myservo_grey;  
Servo myservo_green;

LCD16x2 lcd;

int E1 = 5;     //Port for M1 Speed Control
int E2 = 6;     //Port for M2 Speed Control

int M1 = 4;    //Port for M1 Direction Control
int M2 = 7;    //Port for M2 Direction Control

volatile int CycleCount = 0; // use volatile for shared variables
volatile int MatchLength = 60; // 60 seconds is 1 minutes

void setup() {
  
int b;

  Wire.begin();
  I2C.begin();
  Serial.begin(9600);

pinMode(E1, OUTPUT); //These pins control the motor H-bridge circuitry, so we need to set them up as outputs
pinMode(E2, OUTPUT);
pinMode(M1, OUTPUT);
pinMode(M2, OUTPUT);

mySensor.initSensor();
mySensor.setOperationMode(OPERATION_MODE_NDOF);
mySensor.setUpdateMode(AUTO);


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

arm_and_start(); //call this special function at the end of your setup to run the arming procedure
}

void loop(){ //this code loops continually after running through 'setup' once
  
  const int time1 = millis();
  int time2 = 0, elapsed_time;
  
  while(elapsed_time < 3000){
  
  go_forward(0);
  myservo_green.write(-150);

  time2 = millis();
  elapsed_time = time2 - time1; 

  }

  while(elapsed_time > 3000){
  
  go_forward(248);
  myservo_green.write(-150);

  time2 = millis();
  elapsed_time = time2 - time1; 

  }




}
void drive(int speed){

  analogWrite(E1, speed);
  digitalWrite(M1, HIGH);
  analogWrite(E2, speed);
  digitalWrite(M2, LOW);

}

void steer(int correction){

  int effort;

  if(correction > 180){
    correction = 180;
  }

  if(correction < -180){
    correction = -180;
  }

  if(correction > 0){
    
    effort = map(abs(correction), 0, 180, 0, 255);

    analogWrite(E1, (1/effort) * 255);
    digitalWrite(M1, HIGH);
    analogWrite(E2, 255);
    digitalWrite(M2, LOW);


  }else{
    
    effort = map(abs(correction), 0, 180, 0, 255);

    analogWrite(E1, 255);
    digitalWrite(M1, HIGH);
    analogWrite(E2, (1/effort) * 255);
    digitalWrite(M2, LOW);


  }
}

int signed_angle_fromX_toY(int x, int y){
    int angle;
    int clockwise_angle = y - x;

    if(clockwise_angle > 180){
        angle  = clockwise_angle - 360;

    }else if(clockwise_angle < -180){
        angle = clockwise_angle + 360;
    }else{
        angle = clockwise_angle;
    }

        

    return angle;

}

void go_forward(int target_heading){

  int kp = 1;

  int current_heading = mySensor.readEulerHeading();

  int error = signed_angle_fromX_toY(current_heading, target_heading);

  int correction = kp * error;

  steer(correction);

}




//Don't modify the standard startup and shutdown code below this line!

void arm_and_start() {

int buttons = 0;

float timeleft;

long countdown_int;

long fudge = 0;
bool mode = 0;
long timeflipped = 0;

  Wire.begin();
   
  lcd.lcdClear();
  lcd.lcdSetBlacklight(200);

  lcd.lcdGoToXY(1,2);
  lcd.lcdWrite("v");

  lcd.lcdGoToXY(1,1);
  lcd.lcdWrite("SYNC ");

  lcd.lcdGoToXY(12,1);
  lcd.lcdWrite(" SYNC"); 

  lcd.lcdGoToXY(4,2);
  lcd.lcdWrite("[--set--]");

  
  buttons = lcd.readButtons(); //Sample the state of the large white buttons

do
{
  buttons = lcd.readButtons(); //Sample the state of the large white buttons


  if (buttons == 14) //leftmost button depressed
  {
  fudge = fudge + 450;
  }

  buttons = lcd.readButtons(); //Sample the state of the large white buttons

  if (buttons == 9 && (millis() - timeflipped) > 250) //both middle buttons depressed
  {
  mode = !mode;
  timeflipped = millis();

        if(!mode) //syncing safe mode
        {
          lcd.lcdSetBlacklight(100);
        
         lcd.lcdGoToXY(1,1);
         lcd.lcdWrite("SYNC ");
        
          lcd.lcdGoToXY(12,1);
          lcd.lcdWrite(" SYNC"); 
        
          lcd.lcdGoToXY(4,2);
          lcd.lcdWrite("[--set--]"); 
          
        }
        else if(mode) //armed mode
        {
          lcd.lcdSetBlacklight(400);
          lcd.lcdGoToXY(1,1);
          lcd.lcdWrite("ARMED");
        
         lcd.lcdGoToXY(12,1);
         lcd.lcdWrite("ARMED"); 
        
          lcd.lcdGoToXY(4,2);
          lcd.lcdWrite("[-disarm-]"); 
        }
          
  }


countdown_int = (millis() + fudge) % 10000;

countdown_int = 9998 - countdown_int;

  lcd.lcdGoToXY(7,1);
  lcd.lcdWrite((float) countdown_int/1000, 2);
  
}
while(!(mode == 1 && countdown_int <= 750)); 

  lcd.lcdClear();
  lcd.lcdSetBlacklight(200);
  lcd.lcdGoToXY(6,2);
  lcd.lcdWrite("Begin!");

//Now set up an interupt to trigger after the appropriate number of minutes to shut down the robot
  
 enableInterrupt(11, CheckCycle, RISING); //Attach an interrupt to pin 11
 tone(11, 31); //Output a 31 Hz square wave to pin 11 to trigger this interrupt
}

void CheckCycle(void)
{
    CycleCount = CycleCount + 1;  //Check how many times we've been here
    
  if(CycleCount == 31 * MatchLength) 
  {
  //turn off motors
   analogWrite (E1,0); //Turn off M1
   analogWrite (E2,0); //Turn off M2
      
      while(1) //loop forever to shut down Arduino
      {
      }
  }
}
