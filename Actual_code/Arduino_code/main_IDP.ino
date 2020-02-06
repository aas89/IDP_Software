#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <timer.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
// You can also make another motor on port M2
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);
// Set up scoop motors
Adafruit_DCMotor *pullMotor = AFMS.getMotor(3);
Adafruit_DCMotor *ejectMotor = AFMS.getMotor(4);

auto timer = timer_create_default(); // create a timer with default settings
int timer_count = 0;
char side;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  timer.every(100, inc_timer);
}

/////// GLOBAL THRESHOLDS:
  int leftThreshLineFollow;
  int rightThreshLineFollow;

// the loop routine runs over and over again forever:
void loop() {
  printHelloWorld();
  delayprint(6000);
 
/////// TEST BOX:
//  box(200);

//////// INITIAL TEST OF SENSORS:
//  read_line_sensors();

//////////////////////////// NAVIGATION:

////////////// LINE FOLLOWING INTO CAVE

//// START SEQUENCE (includes automatic calibration):
//  start_sequence();
//  delayprint(1000);
//
////// FIND LINE rotating:
//  rotate_find_line(leftThreshLineFollow, rightThreshLineFollow);
////  delayprint(1000);
//   
////// FOLLOW LINE:
//  follow_line_new (leftThreshLineFollow, rightThreshLineFollow);
//
//////////////// FINDING VICTIMS INSIDE CAVE

///// CHECK IN FRONT FOR VICTIMS
//    bool person_found = sweep_towards_person(); // returns bool of whether person has been found
//    if (person_found) {
//      GO PICK THEM UP
//    }
//    reverse_to_end();

////// FIND SIDE VICTIMS
// while ((people_collected<4) && (millis()<240000)) {
// locate_victim_side;
// sweep_towards_person(); // maybe don't need this??
// pick_up_victim() // needs to increment people_collected within it
// back_to_centre();
//
//}

////// TRIAGE AREA
// FIND START LINE 
// follow_line_new(leftThreshLineFollow, rightThreshLineFollow);
// STOP ON RED LINE???
// DROP OFF VICTIMS
// DRIVE BACK TO START


////// END
////  Serial.println(" -- END");
//  delayprint(1000);
}


// FUNCTIONS:
////////////////////////////////////////////////////////////////////// MAIN FUNCTIONS:

/// FOLLOW WHITE LINE
void follow_line_new(int leftThresh, int rightThresh) {
  Serial.println("Following the line:");
  char *current_pos_wrt_line = "right_on"; // options: left_off, left_on, on_line, right_on, right_off, on_line.
  Serial.println(current_pos_wrt_line);
  delay(2000);
  char *new_pos_wrt_line;
  int defaultSpeed = 90;
  int slowSpeed = 0;
  int new_leftSpeed = defaultSpeed;
  int new_rightSpeed = defaultSpeed;
  int current_leftSpeed = new_leftSpeed;
  int current_rightSpeed = new_rightSpeed;
  forwardvariabledirection(new_leftSpeed, new_rightSpeed);
  int off_count = 0;
  while (true) {
    while ((new_leftSpeed == current_leftSpeed) and (new_rightSpeed == current_rightSpeed)) {
      
      // read the input on analog pin 0 and 1:
      int sensorLeft = analogRead(A0);
      int sensorRight = analogRead(A3);
      // print out the value you read:
      Serial.print("sensorLeft:  ");
      Serial.println(sensorLeft);
      Serial.print("sensorRight:  ");
      Serial.println(sensorRight);
  
     if ((sensorLeft >= leftThresh) && (sensorRight <= rightThresh)) {
      new_pos_wrt_line = "right_on";
      new_leftSpeed = defaultSpeed;
      new_rightSpeed = defaultSpeed;
      off_count = 0;
             

      } else if ((sensorLeft >= leftThresh) && (sensorRight >= rightThresh)) {
        new_pos_wrt_line = "on_line";
        new_leftSpeed = defaultSpeed;
        new_rightSpeed = slowSpeed;
        off_count = 0;
          
      } else if ((sensorLeft <= leftThresh) && (sensorRight >= rightThresh)) {
        new_pos_wrt_line = "left_on";
        new_leftSpeed = defaultSpeed;
        new_rightSpeed = slowSpeed;
        off_count = 0;
        
      } else if ((sensorLeft <= leftThresh) && (sensorRight <= rightThresh)) {
        off_count += 1;
        if (off_count >= 20) {
          rotate_find_line(leftThresh, rightThresh);
          new_pos_wrt_line == "right_on";
          new_leftSpeed = defaultSpeed;
          new_rightSpeed = defaultSpeed;
          off_count = 0;
        } else if ((current_pos_wrt_line == "left_on") || (current_pos_wrt_line == "left_off") || (current_pos_wrt_line == "on_line")) { // Sort of assuming it is going round a right turn, so if both go off at same time, likely that gone off to left.
          new_pos_wrt_line = "left_off";
          new_leftSpeed = defaultSpeed;
          new_rightSpeed = slowSpeed;
        } else if ((current_pos_wrt_line == "right_on") || (current_pos_wrt_line == "right_off")) {
          new_pos_wrt_line =  "right_off";
          new_leftSpeed = slowSpeed;
          new_rightSpeed = defaultSpeed;
        } else {
          Serial.println("This should never happen but just going to carry on (else statement in 2nd while loop of follow_line)");
          new_pos_wrt_line == "right_on";
          new_leftSpeed = defaultSpeed;
          new_rightSpeed = defaultSpeed;
        }
      } else {
        Serial.println("This should never happen but just going to carry on (else statement in 1st while loop of follow_line)");
        new_pos_wrt_line = "right_on";
        new_leftSpeed = defaultSpeed;
        new_rightSpeed = defaultSpeed;
      }
//      if (want to stop follow line put condition in here) {
//        break
//      }
      delay(5);
    }
    forwardvariabledirection(new_leftSpeed, new_rightSpeed);
    current_leftSpeed = new_leftSpeed;
    current_rightSpeed = new_rightSpeed;
    current_pos_wrt_line = new_pos_wrt_line;
//    if (want to stop follow line put condition in here) {
//      break
//    }
  }
}

/*
void follow_line(int leftThresh, int rightThresh) {                                       ////////////// DELETE WHEN follow_line_new WORKS
  Serial.println("Following the line:");
  bool follow_line = true;
  char current_pos_wrt_line = "right_on"; // options: left_off, left_on, on_line, right_on, right_off.
  char new_pos_wrt_line;
  int count_limit = 10000;
  int defaultSpeed = 100;
  int slowSpeed = 0;
  int new_leftSpeed = defaultSpeed;
  int new_rightSpeed = defaultSpeed;
  int current_leftSpeed = new_leftSpeed;
  int current_rightSpeed = new_rightSpeed;
  forwardvariabledirection(new_leftSpeed, new_rightSpeed);
  int count = 0;
  int off_count = 0;
  while (follow_line) {
    while ((new_leftSpeed == current_leftSpeed) and (new_rightSpeed == current_rightSpeed) and (follow_line)) {
      
      // read the input on analog pin 0 and 1:
      int sensorLeft = analogRead(A0);
      int sensorRight = analogRead(A3);
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
      off_count = 0;
             

      } else if ((sensorLeft > leftThresh) && (sensorRight > rightThresh)) {
//        Serial.println("sensorLeft =  on line, sensorRight = on line");
        new_pos_wrt_line = "on_line";
        new_leftSpeed = defaultSpeed;
        new_rightSpeed = slowSpeed;
        off_count = 0;
          
      } else if ((sensorLeft < leftThresh) && (sensorRight > rightThresh)) {
//        Serial.println("sensorLeft =  off, sensorRight = on line");
        new_pos_wrt_line = "left_on";
        new_leftSpeed = defaultSpeed;
        new_rightSpeed = slowSpeed;
        off_count = 0;
        
      } else if ((sensorLeft < leftThresh) && (sensorRight < rightThresh)) {
        off_count += 1;
    //    Serial.println("sensorLeft =  off, sensorRight = off");
        if (off_count >= 20) {
          rotate_find_line(leftThresh, rightThresh);
          new_pos_wrt_line == "right_on";
          new_leftSpeed = defaultSpeed;
          new_rightSpeed = defaultSpeed;
          off_count = 0;
        } else if ((current_pos_wrt_line == "left_on") or (current_pos_wrt_line == "left_off")) {
          new_pos_wrt_line = "left_off";
          new_leftSpeed = defaultSpeed;
          new_rightSpeed = slowSpeed;
        } else if ((current_pos_wrt_line == "right_on") or (current_pos_wrt_line == "right_off")) {
          new_pos_wrt_line =  "right_off";
          new_leftSpeed = slowSpeed;
          new_rightSpeed = defaultSpeed;
        } else if (current_pos_wrt_line == "on_line"){
          rotate_find_line(leftThresh, rightThresh);
          new_pos_wrt_line == "right_on";
          new_leftSpeed = defaultSpeed;
          new_rightSpeed = defaultSpeed;
        } else {
//          rotate_find_line(leftThresh, rightThresh);
//          new_pos_wrt_line == "right_on";
//          new_leftSpeed = defaultSpeed;
//          new_rightSpeed = defaultSpeed;
//          Serial.print("current_pos_wrt_line = ");
//          Serial.println(current_pos_wrt_line);
//          Serial.print("new_pos_wrt_line = ");
//          Serial.println(new_pos_wrt_line);
        }
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
    Serial.println("SPEEDS CHANGING");
    Serial.print("current_pos_wrt_line = ");
    Serial.println(current_pos_wrt_line);
    Serial.print("new_pos_wrt_line = ");
    Serial.println(new_pos_wrt_line);
    forwardvariabledirection(new_leftSpeed, new_rightSpeed);
    current_leftSpeed = new_leftSpeed;
    current_rightSpeed = new_rightSpeed;
    current_pos_wrt_line = new_pos_wrt_line;
  }
  halt();
  Serial.println("Count exceeded or end of line reached so follow line has stopped");
}
*/

/// FIND WHITE LINE rotating
void rotate_find_line (int leftThresh, int rightThresh) {
  variableleftturnangle(300);
  Serial.println("rotating and looking for line:");
  rightturnspeedset(30);
  int count = 0;
  while (count <= 5) {  
  
    // read the input on analog pin 0 and 1:
    int sensorLeft = analogRead(A0);
    int sensorRight = analogRead(A3);
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
//        Serial.println("sensorLeft =  on line, sensorRight = off");
//        Serial.println("C");
      
    } else if ((sensorLeft < leftThresh) && (sensorRight > rightThresh)) {
//        Serial.println("sensorLeft =  off, sensorRight = on line");
//        Serial.println("B");
      
    } else if ((sensorLeft < leftThresh) && (sensorRight < rightThresh)) {
//        Serial.println("sensorLeft =  off, sensorRight = off");
//        Serial.println("A");

    } else {
      Serial.println("error with line detector sensors");
    }
    delay(10);
  }
  halt();  
}

/// LOCATE VICTIM USING SIDE SENSORS AND TURN TO FACE IT
void locate_victim_side() {
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
  bool finished = false;
  int thresh = 100; // Adjust based on reading for wall
  int currentTime = 0;
  if (side == "left") {
    distSensPin = distSenslPin;
  } else {
    distSensPin = distSensrPin;
  }
  Serial.println(thresh);
  forward(80);

  while (!finished) {
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
        delay(2000);
      } else if (repeat == 2) {
        
        Serial.println("Reversing to middle");
        reverse_count = tot_count[0] + 7;
        start_count = timer_count;
        backward(80);
        reversing = true;
        delay(250);
        halt();
        delay(500);
        if (side == "left") {
          leftturn();
        } else if (side == "right") {
          rightturn();
        } else {
          Serial.println("Side variable not working properly");
        }
        pullMotor -> run(BACKWARD);
        pullMotor -> setSpeed(50); 
        delay(1000); // ADAPT SO OPENS ABOUT 90 DEGREES
        
    }
      }
      
    }
    if (reversing) {
      Serial.println(reverse_count);
      Serial.println(timer_count-start_count);
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
      finished = true;
    }
  }
  }
    
  
  distSens = newDistSens; 
  }
  
}


/////////DOESN'T WORK WITH SIDE SENSORS - ADAPT?;
/// MOVE ALONG CENTRE LINE:
void move_along_center(int offCentreThresh, int totalDist) {
  Serial.println("Moving along the centre line:");
  bool along_centre = true;
  int count_limit = 10000;
  int defaultSpeed = 80;
  int slowSpeed = 50;
  int new_leftSpeed = defaultSpeed;
  int new_rightSpeed = defaultSpeed;
  int current_leftSpeed = new_leftSpeed;
  int current_rightSpeed = new_rightSpeed;
  forwardvariabledirection(new_leftSpeed, new_rightSpeed);
  int count = 0;
  while (along_centre) {
    while ((new_leftSpeed == current_leftSpeed) and (new_rightSpeed == current_rightSpeed) and (along_centre)) {
      // read the input on analog pin 0 and 1:
      int sensorDistLeft = analogRead(A0);
      int sensorDistRight = analogRead(A1);
      // print out the value you read:
      Serial.print("sensorDistLeft:  ");
      Serial.println(sensorDistLeft);
      Serial.print("sensorDistRight:  ");
      Serial.println(sensorDistRight);
  
     if ((abs(sensorDistLeft - sensorDistRight) <= offCentreThresh)) {
//      Serial.println("on centre");
      new_leftSpeed = defaultSpeed;
      new_rightSpeed = defaultSpeed;
             

      } else if ((sensorDistLeft - sensorDistRight) < (-offCentreThresh)) {
//        Serial.println("too far left");
        new_leftSpeed = defaultSpeed;
        new_rightSpeed = slowSpeed;
          
      } else if ((sensorDistLeft - sensorDistRight) > (offCentreThresh)) {
//        Serial.println("too far right");
        new_leftSpeed = slowSpeed;
        new_rightSpeed = defaultSpeed;
      } else {
        Serial.println("error with move_along_sensor");
      }
      delay(10);
      count += 1;
      if (count >= count_limit) {
        Serial.println("Count exceeded");
        along_centre = false;
      }
    }
    forwardvariabledirection(new_leftSpeed, new_rightSpeed);
    current_leftSpeed = new_leftSpeed;
    current_rightSpeed = new_rightSpeed;
  }
  halt();
  Serial.println("Count exceeded so follow line has stopped");
}

////////////// SCOOP FUNCTIONS

///// PICK UP PEOPLE
void pick_up_victim(){
//  GO TOWARDS PERSON AND STOP WITHIN 5CM
  delay(5000); // WAIT FOR VICTIM IDENTIFICATION CIRCUIT TO WORK
  forward(50); // CHANGE SPEED TO LOWEST POSSIBLE THAT STILL MOVES
  pullMotor -> run(BACKWARD);
  pullMotor -> setSpeed(60);
  delay(1000); // CHANGE SO STOPS WHEN CLOSED
  people_collected += 1;
  }

void drop_off_victims () {
  rightturn();
  backward(80);
  delay(500);
  pullMotor -> run(FORWARD);
  pullMotor -> setSpeed(60);
  delay(1000);
  ejectMotor -> run(FORWARD);
  ejectMotor -> setSpeed(40);
  delay(2000); // OPEN MORE THAN 90 DEGREES?
  ejectMotor -> run(BACKWARD);
  ejectMotor -> setSpeed(50);
  delay(2000);
  pullMotor -> run(BACKWARD);
  pullMotor -> setSpeed(60);
  delay(1000);
  rightturn();
  
}

//////////////////////////////////////////////////////////////////////// SEQUENCES:

void start_sequence() {
  Serial.println("start_sequence");
  automatic_calibration();
  forwardvariabledirection(150, 140);
  delay(1000);
  halt();
}

//////////////////////////////////////////////////////////////////////// MOTION FUNCTIONS:

void forward(int speedSet) {
  Serial.println("forward");
  myMotor1->run(FORWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(speedSet);
  myMotor2->setSpeed(speedSet);
}


void forwardvariabledirection(int leftSpeed, int rightSpeed) {
  Serial.println("forwardvariabledirection");
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

void backwardvariabledirection(int leftSpeed, int rightSpeed) {
  Serial.println("forward");
  myMotor1->run(BACKWARD);
  myMotor2->run(BACKWARD);
  myMotor1->setSpeed(leftSpeed);
  myMotor2->setSpeed(rightSpeed);
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

void box(int speedSet) {
  Serial.println("boxing");
  accelerateforward(speedSet);
  delayprint(1000);
  decelerateforward(speedSet);

  leftturn();

  accelerateforward(speedSet);
  delayprint(1000);
  decelerateforward(speedSet);

  rightturn();

  acceleratebackward(speedSet);
  delayprint(1000);
  deceleratebackward(speedSet);

  leftturn();

  acceleratebackward(speedSet);
  delayprint(1000);
  deceleratebackward(speedSet);

  rightturn();
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

void rightturnspeedset(int speedSet) {
  Serial.println("rightturn");
  myMotor1->run(FORWARD);
  myMotor2->run(BACKWARD);
  myMotor1->setSpeed(speedSet);
  myMotor2->setSpeed(speedSet);
}

//////////////////////////////////////////////////////////////////// MISCELLANEOUS FUNCTIONS:

void printHelloWorld() {
  Serial.println("Hello worldlings");
}

void delayprint(int i) {
  Serial.print("delay:   ");
  Serial.println(i);
  delay(i);
}

/// TIMER INCREMENTS
int inc_timer() {
  timer_count += 1;
}

void read_line_sensors() {
    int sensorLeft = analogRead(A0);
    int sensorRight = analogRead(A3);
    // print out the value you read:
    Serial.print("sensorLeft:  ");
    Serial.println(sensorLeft);
    Serial.print("sensorRight:  ");
    Serial.println(sensorRight);
}

void manual_calibration() {
  Serial.println("Manual Calibration");
  Serial.println("Position with both line detectors OFF the line");
  delayprint(10000);
  Serial.println("Taking OFF line readings now...");
  int offLeft1 = analogRead(A0);
  int offRight1 = analogRead(A3);
  Serial.println("... readings taken");
  Serial.println("Position with both line detectors OFF the line again");
  delayprint(10000);
  Serial.println("Taking OFF line readings again now...");
  int offLeft2 = analogRead(A0);
  int offRight2 = analogRead(A3);
  Serial.println("... readings taken");
  Serial.println("Position with both line detectors ON the line");
  delayprint(10000);
  Serial.println("Taking ON line readings now...");
  int onLeft1 = analogRead(A0);
  int onRight1 = analogRead(A3);
  Serial.println("... readings taken");
  Serial.println("Position with both line detectors ON the line again");
  delayprint(10000);
  Serial.println("Taking ON line readings again now...");
  int onLeft2 = analogRead(A0);
  int onRight2 = analogRead(A3);
  Serial.println("... readings taken");
  int aveOffLeft = round((offLeft1+offLeft2)/2.0);
  int aveOffRight = round((offRight1+offRight2)/2.0);
  int aveOnLeft = round((onLeft1+onLeft2)/2.0);
  int aveOnRight = round((onRight1+onRight2)/2.0);
  int potLeftThresh = round((offLeft1+offLeft2+onLeft1+onLeft2)/4.0);
  int potRightThresh = round((offRight1+offRight2+onRight1+onRight2)/4.0);
  Serial.println("RESULTS:");
  Serial.println("Left then right in form: offRight1, offRight2, onRight1, onRight2");
  Serial.println(offLeft1);
  Serial.println(offLeft2);
  Serial.println(onLeft1);
  Serial.println(onLeft2);
  Serial.println(offRight1);
  Serial.println(offRight2);
  Serial.println(onRight1);
  Serial.println(onRight2);
  Serial.print("Average OFF left = ");
  Serial.println(aveOffLeft);
  Serial.print("Average ON left = ");
  Serial.println(aveOnLeft);
  Serial.print("Average OFF right = ");
  Serial.println(aveOffRight);
  Serial.print("Average ON right = ");
  Serial.println(aveOnRight);
  Serial.println("Calculated potential thresholds:  ");
  Serial.print("Left threshold = ");
  Serial.println(potLeftThresh);
  Serial.print("Right threshold = ");
  Serial.println(potRightThresh);  
}

////// AUTOMATIC CALIBRATION:
int automatic_calibration(){
  Serial.println("Automatic Calibration");
  Serial.println("Taking readings...");
  int onLeft = analogRead(A0);
  int onRight = analogRead(A3);
  delay(100);
  leftturn();
  halt();
  delay(100);
  int offLeft1 = analogRead(A0);
  int offRight1 = analogRead(A3);
  delay(100);
  rightturn();
  halt();
  delay(100);
  rightturn();
  halt();
  delay(100);
  int offLeft2 = analogRead(A0);
  int offRight2 = analogRead(A3);
  delay(100);
  leftturn();
  delay(100);
  Serial.println("... readings taken");
  leftThreshLineFollow = round((offLeft1+offLeft2+onLeft*2.0)/4.0);
  rightThreshLineFollow = round((offRight1+offRight2+onRight*2.0)/4.0);
  Serial.println("RESULTS:");
  Serial.println("onLeft, offLeft1, offLeft2, --- onRight, onRight1, onRight2");
  Serial.println(onLeft);
  Serial.println(offLeft1);
  Serial.println(offLeft2);
  Serial.println(onRight);
  Serial.println(offRight1);
  Serial.println(offRight2);
  Serial.println("Calculated potential thresholds:  ");
  Serial.print("Left threshold = ");
  Serial.println(leftThreshLineFollow);
  Serial.print("Right threshold = ");
  Serial.println(rightThreshLineFollow);  
}
