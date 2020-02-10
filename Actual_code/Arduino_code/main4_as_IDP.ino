///////// NEED TO ADD FLASHING AMBER LIGHT WHEN MOVING AND THEN LIT FOR 1 SEC WHEN STOPPED IN FRONT OF VICTIM


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

#include <Ultrasonic.h>

Ultrasonic ultrasonic1(10, 11); // 10=yellow=trig, 11=green=echo
Ultrasonic ultrasonic2(12, 13); // 12=yellow, 13=black

auto timer_s = timer_create_default(); // create a timer with default settings
auto timer_a = timer_create_default(); // create a timer with default settings
int timer_count_s = 0;
int time_counter_a;
char side;
int people_collected = 0;
int turns = 0;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  timer_s.every(100, inc_timer);
  timer_a.every(100, increment_time_counter);
}

/////// GLOBAL THRESHOLDS:
  int leftThreshLineFollow;
  int rightThreshLineFollow;
  int extraThreshLineFollow;

// the loop routine runs over and over again forever:
void loop() {
  printHelloWorld();
  delayprint(6000);


//rightturn();
//delay(10000);

/////// TEST BOX:
//  box(200);

//////// INITIAL TEST OF SENSORS:
//  read_line_sensors();
//  read_ultrasound_sensors();
//  delay(1000);

//////////////////////////// NAVIGATION:

//////////////////////////// LINE FOLLOWING INTO CAVE:

//// START SEQUENCE (includes automatic calibration):
//  start_sequence();
//  delayprint(1000);
//////
//////////// FIND LINE rotating:
//  rotate_find_line(leftThreshLineFollow, rightThreshLineFollow);
//  delayprint(1000);
//////
//////////// FOLLOW LINE:
//  follow_line_new (leftThreshLineFollow, rightThreshLineFollow, extraThreshLineFollow);
//
////////////////////////////////// FINDING VICTIMS INSIDE CAVE:
/////// CHECK IN FRONT FOR VICTIMS
    bool person_found = sweep_towards_person(); // returns bool of whether person has been found
    if (person_found == true) {
      bool person_realigned = sweep_realign();
      if (person_realigned == true) {
        Serial.println("picking up victim (after delay) ....");
        delayprint(3000);
        pick_up_victim();
      }
    }
    Serial.println("Reversing to end (after delay) ....");
    delayprint(3000);
    reverse_to_end();
//
//////// FIND SIDE VICTIMS
////   while ((people_collected<4) && (millis()<240000)) { // <---------------------------------------------- check this millis() variable is actually getting updated / working
        locate_victim_side();
     person_found = sweep_towards_person(); // maybe don't need this??
     if (person_found) {
          pick_up_victim();  // needs to increment people_collected within it
        }
     back_to_centre();
//  }
if (turns == 1) {
  rightturn();
  rightturn();
}

//////////////////////////// DROP OFF + BACK TO START
  find_line_on_way_back(leftThreshLineFollow, rightThreshLineFollow, extraThreshLineFollow);
  delayprint(1000);
  
  rotate_find_line(leftThreshLineFollow, rightThreshLineFollow);
  delayprint(1000);
  follow_line_new (leftThreshLineFollow, rightThreshLineFollow, extraThreshLineFollow);
//  
  drop_off_victims();
  reverse_to_start(leftThreshLineFollow, rightThreshLineFollow, extraThreshLineFollow);
//
  Serial.println(" --------------- END ----------------");
  delayprint(20000);


/////////////////////////////// END  
}


// FUNCTIONS:
////////////////////////////////////////////////////////////////////// MAIN FUNCTIONS:

/// FOLLOW WHITE LINE
void follow_line_new(int leftThresh, int rightThresh, int extraThresh) {
  Serial.println("Following the line:");
  char *current_pos_wrt_line = "right_on"; // options: left_off, left_on, on_line, right_on, right_off, on_line.
  Serial.println(current_pos_wrt_line);
  delay(2000);
  char *new_pos_wrt_line;
  int defaultSpeed = 100;
  int slowSpeed = 0;
  int new_leftSpeed = defaultSpeed;
  int new_rightSpeed = defaultSpeed;
  int current_leftSpeed = new_leftSpeed;
  int current_rightSpeed = new_rightSpeed;
  forwardvariabledirection(new_leftSpeed, new_rightSpeed);
  int off_count = 0;
  bool following_line = true;
  while (following_line) {
    while ((new_leftSpeed == current_leftSpeed) and (new_rightSpeed == current_rightSpeed)) {
      
      // read the input on analog pin 0 and 1:
      int sensorLeft = analogRead(A2);
      int sensorRight = analogRead(A0);
      int sensorExtra = analogRead(A1);
      // print out the value you read:
      Serial.print("sensorLeft:  ");
      Serial.println(sensorLeft);
      Serial.print("sensorRight:  ");
      Serial.println(sensorRight);
  
     if ((sensorLeft >= leftThresh) && (sensorRight >= rightThresh) && (sensorExtra >= extraThresh) && (millis()>25000)) {
      Serial.print("reached end");
      halt();
      following_line = false;
      delayprint(1000);
      break;
     } else if ((sensorLeft >= leftThresh) && (sensorRight <= rightThresh)) {
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
    if (following_line == false) {
      break;
    }
  }
}


/// FIND WHITE LINE rotating
void rotate_find_line (int leftThresh, int rightThresh) {
  variableleftturnangle(500);
  Serial.println("rotating and looking for line:");
  rightturnspeedset(50);
  int count = 0;
  while (count <= 5) {  
  
    // read the input on analog pin 0 and 1:
    int sensorLeft = analogRead(A2);
    int sensorRight = analogRead(A0);
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
  int distSensPin = A3;
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
  int thresh = 90; // Adjust based on reading for wall
  int currentTime = 0;
  Serial.println(thresh);
  forward(80);

  while (!finished) {
  timer_s.tick(); // tick the timer
  int ultrasonic1reading = ultrasonic1.read();
  int ultrasonic2reading = ultrasonic2.read();
  Serial.println(ultrasonic1reading);
  Serial.println(ultrasonic2reading);
  if ((ultrasonic2reading > 80) && turns==0) {
    rightturn();
    delay(500);
    rightturn();
    turns += 1;
    forward(80);
   } else if ((ultrasonic1reading < 30) && (turns==1)) {
    rightturn();
    delay(500);
    rightturn();
    turns += 1;
    forward(80);
   }
  newDistSens = analogRead(distSensPin); // Read sensor
  currentTime = timer_count_s;
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
      start_count = timer_count_s;
      i = 0;
    }
  } else if (found) {
    if (i<3) {
      Serial.println("Adding 1 to i");
    i += 1;
    } else if (i==3) {
      end_count = timer_count_s;
      Serial.println("Reached end of person");
      i = 0;
      tot_count[repeat] = end_count-start_count;
      found = false;
      repeat += 1;
      if (repeat == 1) {
        Serial.println("Reversing to start");
        reverse_count = tot_count[0] + 7;
        start_count = timer_count_s;
        backward(80);
        reversing = true;
        delay(2000);
      } else if (repeat == 2) {
        
        Serial.println("Reversing to middle");
        reverse_count = tot_count[0] + 7;
        start_count = timer_count_s;
        backward(80);
        reversing = true;
        delay(250);
        halt();
        delay(500);
        rightturn();

        
    }
      }
      
    }
    if (reversing) {
      Serial.println(reverse_count);
      Serial.println(timer_count_s-start_count);
      if (((timer_count_s-start_count) < reverse_count) && repeat==1) {
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

////////////// GO TOWARDS PERSON
bool sweep_towards_person() {
  pullMotor -> run(BACKWARD);
  pullMotor -> setSpeed(200); 
  delay(1000); // ADAPT SO OPENS ABOUT 90 DEGREES
  pullMotor -> setSpeed(0);
  bool person_found = false;
  time_counter_a = 0;
  variableleftturnangle(400);
  delayprint(1000);
  int sweep_direction_count = 0;
  while ((sweep_direction_count < 10) and (person_found == false)) {
    Serial.print("sweep_direction_count = ");
    Serial.println(sweep_direction_count);
    delayprint(2000);
    //// start sweep:
    if (sweep_direction_count%2 == 0) {
      sweep_direction_count += 1;
      rightturnspeedset(50);      
    } else if (sweep_direction_count%2 == 1) {
      sweep_direction_count += 1;
      leftturnspeedset(50);      
    } else {
      Serial.println("error with sweep_direction_count");
    }
    int sweep_start_timer = time_counter_a;
    int current_ultrasound = ultrasonic1.read();
    int previous_ultrasound = ultrasonic1.read();
    int pre_prev_ultrasound = ultrasonic1.read();
    int close_ultrasound = 0;
    int close_counter = 0;
    while (time_counter_a < sweep_start_timer + 50) {
      timer_a.tick(); // tick the timer
      current_ultrasound = ultrasonic1.read();
      Serial.println(current_ultrasound);
      if ((((previous_ultrasound-current_ultrasound) >= 20)) and ((pre_prev_ultrasound-current_ultrasound) >= 20)) {
        close_ultrasound = current_ultrasound;
        close_counter = 1;
      } else if ((abs(current_ultrasound-close_ultrasound) <= 5) and (close_ultrasound != 0)) {
        close_counter += 1;
      } else {
        close_counter = 0;
        close_ultrasound = 0;
      }
      if (close_counter >= 6) {
        delay(950);
        halt();
        Serial.print("Person Located, distance to person (in cm?) = ");
        Serial.println(current_ultrasound);
        Serial.print("Collecting person...");
        delayprint(2000);
        forward(100);
        delayprint(current_ultrasound * 50);  /// <---------------------- CHANGE DISTANCE CALIBRATION FACTOR HERE
        halt();
        person_found = true;
        return true;
        break;
      }
      pre_prev_ultrasound = previous_ultrasound;
      previous_ultrasound = current_ultrasound;
    }
    halt();
    delayprint(1000);
  }
  Serial.print("Reached end of sweeping loop");
  delayprint(2000);
  return false;  
}



bool sweep_realign() {
  bool person_found = false;
  time_counter_a = 0;
  variableleftturnangle(400);
  delayprint(1000);
  int sweep_direction_count = 0;
  while ((sweep_direction_count < 10) and (person_found == false)) {
    Serial.print("sweep_direction_count = ");
    Serial.println(sweep_direction_count);
    delayprint(2000);
    //// start sweep:
    if (sweep_direction_count%2 == 0) {
      sweep_direction_count += 1;
      rightturnspeedset(50);      
    } else if (sweep_direction_count%2 == 1) {
      sweep_direction_count += 1;
      leftturnspeedset(50);      
    } else {
      Serial.println("error with sweep_direction_count");
    }
    int sweep_start_timer = time_counter_a;
    int current_ultrasound = ultrasonic1.read();
    int previous_ultrasound = ultrasonic1.read();
    int pre_prev_ultrasound = ultrasonic1.read();
    int close_ultrasound = 0;
    int close_counter = 0;
    while (time_counter_a < sweep_start_timer + 50) {
      timer_a.tick(); // tick the timer
      current_ultrasound = ultrasonic1.read();
      Serial.println(current_ultrasound);
      if ((((previous_ultrasound-current_ultrasound) >= 20)) and ((pre_prev_ultrasound-current_ultrasound) >= 20)) {
        close_ultrasound = current_ultrasound;
        close_counter = 1;
      } else if ((abs(current_ultrasound-close_ultrasound) <= 5) and (close_ultrasound != 0)) {
        close_counter += 1;
      } else {
        close_counter = 0;
        close_ultrasound = 0;
      }
      if (close_counter >= 6) {
        delay(950);
        halt();
        Serial.print("Person Located, distance to person (in cm?) = ");
        Serial.println(current_ultrasound);
        person_found = true;
        return true;
        break;
      }
      pre_prev_ultrasound = previous_ultrasound;
      previous_ultrasound = current_ultrasound;
    }
    halt();
    delayprint(1000);
  }
  Serial.print("Reached end of sweeping loop");
  delayprint(2000);
  return false;  
}

