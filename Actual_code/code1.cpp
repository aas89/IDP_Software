#include <Wire.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
// You can also make another motor on port M2
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
}

// the loop routine runs over and over again forever:
void loop() {
  printHelloWorld();
  delayprint(4000);
  
// FOLLOW LINE:
  follow_line(100, 100);

};


// FUNCTIONS:


////////////// MISCELLANEOUS FUNCTIONS:

void printHelloWorld() {
  Serial.println("Hello worldlings");
}

void delayprint(int i) {
  Serial.print("delay:   ");
  Serial.println(i);
  delay(i);
}


////////////// MOTION FUNCTIONS:

void forward(int speedSet) {
  Serial.println("forward");
  myMotor1->run(FORWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(speedSet);
  myMotor2->setSpeed(speedSet);
}


void forwardvariabledirection(int leftSpeed, int rightSpeed) {
  Serial.println("forward");
  myMotor1->run(FORWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(leftSpeed);
  myMotor2->setSpeed(rightSpeed);
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


void accelerateforward(int speedSet) {
  myMotor1->run(FORWARD);
  myMotor2->run(FORWARD);
  int i = 0;
  Serial.println("accellerating_forward");
  for (i=0; i<=speedSet; i++) {
    myMotor1->setSpeed(i);
    myMotor2->setSpeed(i);  
    delay(2);
  }
}

void acceleratebackward(int speedSet) {
  myMotor1->run(BACKWARD);
  myMotor2->run(BACKWARD);
  int i = 0;
  Serial.println("accellerating_backward");
  for (i=0; i<=speedSet; i++) {
    myMotor1->setSpeed(i);
    myMotor2->setSpeed(i);  
    delay(2);
  }
}


void decelerateforward(int speedSet) {
  myMotor1->run(FORWARD);
  myMotor2->run(FORWARD);
  int i = 0;
  Serial.println("decelerating_forward");
  for (i=speedSet; i!=0; i--) {
    myMotor1->setSpeed(i);
    myMotor2->setSpeed(i);  
    delay(2);
  }
}

void deceleratebackward(int speedSet) {
  myMotor1->run(BACKWARD);
  myMotor2->run(BACKWARD);
  int i = 0;
  Serial.println("decelerating_backward");
  for (i=speedSet; i!=0; i--) {
    myMotor1->setSpeed(i);
    myMotor2->setSpeed(i);  
    delay(5);
  }
}

void leftturn() {
  Serial.println("leftturn");
  myMotor1->run(BACKWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(165);
  myMotor2->setSpeed(165);
  delay(1500);
  myMotor1->setSpeed(0);
  myMotor2->setSpeed(0);
}

void rightturn() {
  Serial.println("rightturn");
  myMotor1->run(FORWARD);
  myMotor2->run(BACKWARD);
  myMotor1->setSpeed(165);
  myMotor2->setSpeed(165);
  delay(1500);
  myMotor1->setSpeed(0);
  myMotor2->setSpeed(0);
}

void variableleftturnangle(int delayangle) {
  Serial.println("leftturn");
  myMotor1->run(BACKWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(165);
  myMotor2->setSpeed(165);
  delay(delayangle);
  myMotor1->setSpeed(0);
  myMotor2->setSpeed(0);
}

void leftturnspeedset(int speedSet) {
  Serial.println("leftturn");
  myMotor1->run(BACKWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(speedSet);
  myMotor2->setSpeed(speedSet);
}




////////////// HARDCODED SEQUENCE FUNCTIONS:
/// START SEQUENCE
void start_sequence() {
  accelerateforward(250);
  delayprint(5000);
  decelerateforward(250);

  rightturn();

  Serial.println("Taking threshold readings");
  int sensorLeft = analogRead(A0);
  int sensorRight = analogRead(A1);
  // THRESHOLDS:
  int leftThresh = sensorLeft + 20;
  int rightThresh = sensorRight + 20;
  delayprint(3000);
  
  accelerateforward(250);
  delayprint(3000);
  decelerateforward(250);

// LOOK FOR LINE AND STOP:
  stop_at_line(leftThresh, rightThresh);
}

/// BACK TO CAVE SEQUENCE


////////////// SENSOR FUNCTIONS:
/// STOP AT WHITE LINE:
void stop_at_line(int leftThresh, int rightThresh) {
  Serial.println("looking for line:");
  forward(80);
  int count = 0;
  while (count <= 5) {  
  
    // read the input on analog pin 0 and 1:
    int sensorLeft = analogRead(A0);
    int sensorRight = analogRead(A1);
    // print out the value you read:
    Serial.print("sensorLeft:  ");
    Serial.println(sensorLeft);
    Serial.print("sensorRight:  ");
    Serial.println(sensorRight);

   if ((sensorLeft > leftThresh) && (sensorRight > rightThresh)) {
  //    Serial.println("sensorLeft =  on line, sensorRight = on line");
      Serial.println("LINE REACHED!");
      count += 1;
      Serial.print("Count:  ");
      Serial.println(count);
     
    } else if ((sensorLeft > leftThresh) && (sensorRight < rightThresh)) {
  //    Serial.println("sensorLeft =  on line, sensorRight = off");
      
    } else if ((sensorLeft < leftThresh) && (sensorRight > rightThresh)) {
  //    Serial.println("sensorLeft =  off, sensorRight = on line");
      
    } else if ((sensorLeft < leftThresh) && (sensorRight < rightThresh)) {
  //    Serial.println("sensorLeft =  off, sensorRight = off");
      
    } else if ((sensorLeft == 0) or (sensorRight == 0)) {
      Serial.println("one of the photodiode sensors is not connected properly");
    } else {
      Serial.println("error with photodiode sensors");
    }
    delay(10);
  }

  halt();
  
}

/// FOLLOW WHITE LINE
void follow_line(int leftThresh, int rightThresh) {
  Serial.println("Following the line:");
  bool follow_line = true;
  char current_pos_wrt_line = "right_on"; // options: left_off, left_on, on_line, right_on, right_off.
  char new_pos_wrt_line;
  int count_limit = 10000;
  int defaultSpeed = 80;
  int slowSpeed = 50;
  int new_leftSpeed = defaultSpeed;
  int new_rightSpeed = defaultSpeed;
  int current_leftSpeed = new_leftSpeed;
  int current_rightSpeed = new_rightSpeed;
  forwardvariabledirection(new_leftSpeed, new_rightSpeed);
  int count = 0;
  while (follow_line) {
    while ((new_leftSpeed == current_leftSpeed) and (new_rightSpeed == current_rightSpeed) and (follow_line)) {
      
      // read the input on analog pin 0 and 1:
      int sensorLeft = analogRead(A0);
      int sensorRight = analogRead(A1);
      // print out the value you read:
      Serial.print("sensorLeft:  ");
      Serial.println(sensorLeft);
      Serial.print("sensorRight:  ");
      Serial.println(sensorRight);
  
     if ((sensorLeft > leftThresh) && (sensorRight < rightThresh)) {
//      Serial.println("sensorLeft =  on line, sensorRight = off");
      new_pos_wrt_line = "right_on";
      new_leftSpeed = defaultSpeed;
      new_rightSpeed = defaultSpeed;
             

      } else if ((sensorLeft > leftThresh) && (sensorRight > rightThresh)) {
//        Serial.println("sensorLeft =  on line, sensorRight = on line");
        new_pos_wrt_line = "on_line";
        new_leftSpeed = defaultSpeed;
        new_rightSpeed = slowSpeed;
          
      } else if ((sensorLeft < leftThresh) && (sensorRight > rightThresh)) {
//        Serial.println("sensorLeft =  off, sensorRight = on line");
        new_pos_wrt_line = "left_on";
        new_leftSpeed = defaultSpeed;
        new_rightSpeed = slowSpeed;
        
      } else if ((sensorLeft < leftThresh) && (sensorRight < rightThresh)) {
    //    Serial.println("sensorLeft =  off, sensorRight = off");
        if ((current_pos_wrt_line == "left_on") or (current_pos_wrt_line == "left_off")) {
          new_pos_wrt_line = "left_off";
          new_leftSpeed = defaultSpeed;
          new_rightSpeed = slowSpeed;
        } else if ((current_pos_wrt_line == "right_on") or (current_pos_wrt_line == "right_off")) {
          new_pos_wrt_line = "right_off";
          new_leftSpeed = slowSpeed;
          new_rightSpeed = defaultSpeed;
        } else if (current_pos_wrt_line == "on_line"){
          Serial.println("think reached end of white line");
          follow_line = false;
        } else {
          Serial.println("lost line - i.e. something went wrong as this should not happen.");
        }
      } else if ((sensorLeft == 0) or (sensorRight == 0)) {
        Serial.println("one of the photodiode sensors is not connected properly");
      } else {
        Serial.println("error with photodiode sensors");
      }
      delay(10);
      count += 1;
      if (count >= count_limit) {
        Serial.println("Count exceeded");
        follow_line = false;
      }
    }
    forwardvariabledirection(new_leftSpeed, new_rightSpeed);
    current_leftSpeed = new_leftSpeed;
    current_rightSpeed = new_rightSpeed;
    current_pos_wrt_line = new_pos_wrt_line;
  }
  halt();
  Serial.println("Count exceeded so follow line has stopped");
}

/// FIND WHITE LINE rotating
void rotate_find_line (int leftThresh, int rightThresh) {
  variableleftturnangle(500);
  Serial.println("rotating and looking for line:");
  leftturnspeedset(40);
  int count = 0;
  while (count <= 5) {  
  
    // read the input on analog pin 0 and 1:
    int sensorLeft = analogRead(A0);
    int sensorRight = analogRead(A1);
    // print out the value you read:
    Serial.print("sensorLeft:  ");
    Serial.println(sensorLeft);
    Serial.print("sensorRight:  ");
    Serial.println(sensorRight);
    if ((sensorLeft > leftThresh) && (sensorRight < rightThresh)) {
   //    Serial.println("sensorLeft =  on line, sensorRight = on line");
      Serial.println("CORRECT SIDE OF LINE REACHED!");
      count += 1;
      Serial.print("Count:  ");
      Serial.println(count);
     
    } else if ((sensorLeft > leftThresh) && (sensorRight > rightThresh)) {
  //    Serial.println("sensorLeft =  on line, sensorRight = off");
        Serial.println("C");
      
    } else if ((sensorLeft < leftThresh) && (sensorRight > rightThresh)) {
  //    Serial.println("sensorLeft =  off, sensorRight = on line");
        Serial.println("B");
      
    } else if ((sensorLeft < leftThresh) && (sensorRight < rightThresh)) {
  //    Serial.println("sensorLeft =  off, sensorRight = off");
        Serial.println("A");
      
    } else if ((sensorLeft == 0) or (sensorRight == 0)) {
      Serial.println("one of the line detector sensors is not connected properly");
    } else {
      Serial.println("error with line detector sensors");
    }
    delay(10);
  }
  halt();
  
  
}