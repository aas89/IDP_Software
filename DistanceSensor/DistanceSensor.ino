#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <timer.h>


int distSenslPin = A0; // select correct input pin
int distSensrPin = A1;
int distSensPin;
int distSens = 0;
int newDistSens = 0;
int i = 0;
int start_count = 0;
int end_count = 0;
int reverse_count = 0;
int repeat = 0;
int *tot_count;
bool found = false;
bool reversing = false;
int thresh = 100; // Adjust based on reading for wall
auto timer = timer_create_default(); // create a timer with default settings
int timer_count = 0;
int currentTime = 0;
char side = "left";

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
// You can also make another motor on port M2
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);

int inc_timer() {
  timer_count += 1;
}

void setup() {
  Serial.begin(9600);
  AFMS.begin();  // create with the default frequency 1.6KHz
  // increment timer count every 1000 millis (1 second)
  timer.every(100, inc_timer);
  if (side == "left") {
    distSensPin = distSenslPin;
  } else {
    distSensPin = distSensrPin;
  }
  thresh = analogRead(distSensPin);
  Serial.println(thresh);
  forward(80);
}

void loop() {
  timer.tick(); // tick the timer
  newDistSens = analogRead(distSensPin); // Read sensor
  currentTime = timer_count;
  Serial.print("Sensor Reading is: "); 
  Serial.println(newDistSens);
  Serial.print("Found Status is: ");
  Serial.println(found);
  Serial.print("Repeat Number is: ");
  Serial.println(repeat);
  Serial.print("Current time in 100 milliseconds is: ");
  Serial.println(currentTime);
  if ((newDistSens - thresh >= 90) && !(reversing)) {
    if (i<3) {
      Serial.println("Adding 1 to i");
      i += 1;
    } else if (i==3) {
      found = true;
      start_count = timer_count;
      i = 0;
    }
  } else if (found) {
    if (i<3) {
      Serial.println("Adding 1 to i");
    i += 1;
    } else if (i==3) {
      end_count = timer_count;
      Serial.println("Reached end of person");
      i = 0;
      tot_count[repeat] = end_count-start_count;
      found = false;
      repeat += 1;
      if (repeat == 1) {
        Serial.println("Reversing to start");
        reverse_count = tot_count[0] + 7;
        start_count = timer_count;
        backward(80);
        reversing = true;
      } else if (repeat == 2) {
        
        Serial.println("Reversing to start");
        reverse_count = tot_count[0] + 7;
        start_count = timer_count;
        backward(80);
        reversing = true;
        delay(1000);
        halt();
        delay(500);
        rightturn();
        
    }
      }
      
    }
    if (reversing) {
      if (((timer_count-start_count) < reverse_count) && repeat==1) {
        Serial.println("Still Reversing");
    
  } else {
      halt();
    if (repeat == 1) {
      Serial.println("Going forwards again");
      forward(80);
      reversing = false;
    } else {
      Serial.println("Finished");
    }
  }
  }
    
  
  distSens = newDistSens;
  delay(200);
}

void forward(int speedSet) {
  Serial.println("forward");
  myMotor1->run(FORWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(speedSet);
  myMotor2->setSpeed(speedSet);
}

void backward(int speedSet) {
  Serial.println("backward");
  myMotor1->run(BACKWARD);
  myMotor2->run(BACKWARD);
  myMotor1->setSpeed(speedSet);
  myMotor2->setSpeed(speedSet);
}

void halt() {
  Serial.println("halt");
  myMotor1->setSpeed(0);
  myMotor2->setSpeed(0);
}


void rightturn() {
  Serial.println("rightturn");
  myMotor1->run(FORWARD);
  myMotor2->run(BACKWARD);
  myMotor1->setSpeed(165);
  myMotor2->setSpeed(165);
  delay(1400);
  myMotor1->setSpeed(0);
  myMotor2->setSpeed(0);
}
