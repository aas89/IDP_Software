///////////// FINAL CODE FOR COMPETITION!!!

// Import all necessary libraries
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <timer.h>
#include <Ultrasonic.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1);
// You can also make another motor on port M2
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2);
// Set up scoop motors
Adafruit_DCMotor *pullMotor = AFMS.getMotor(3);
Adafruit_DCMotor *ejectMotor = AFMS.getMotor(4);


// Initialise ultrasound sensors at digital pins 9-12
Ultrasonic ultrasonic1(9, 10); 
Ultrasonic ultrasonic2(11, 12); 

// Initialise timers
auto timer_s = timer_create_default(); // create a timer with default settings
auto timer_a = timer_create_default(); // create a timer with default settings
// Initialise global variables
int timer_count_s = 0;
int time_counter_a;
char side; // Unused
int people_collected = 0; // How many people have been collected so far
int turns = 0; // Keeps track of where robot is in search, and so which direction it's facing
const int ledPin =  13; // the number of the LED pin for amber LED
int ledState = LOW; // ledState used to set the LED for amber LED
bool blink = false; // For blinking amber LED
unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = 500; // Sets frequency of blinking of amber LED
bool looping = true; // Determines whether to go back for another person


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  
  // Sets up timers increments
  timer_s.every(100, inc_timer);
  timer_a.every(100, increment_time_counter);
    
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
}

/////// GLOBAL THRESHOLDS:
//  int leftThreshLineFollow = 640;
//  int rightThreshLineFollow = 20;
//  int extraThreshLineFollow = 280;

//  int leftThreshLineFollow = 1;
//  int rightThreshLineFollow = 1;
//  int extraThreshLineFollow = 1;

// Thresholds for line following
  int leftThreshLineFollow;
  int rightThreshLineFollow;
  int extraThreshLineFollow;

// the loop routine runs over and over again forever:
void loop() {
  // Takes intial reading of time in milliseconds, will be used to check how long the robot has left
  unsigned long currentMillis = millis();
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
  start_sequence();
  delay(200);
////////
while (looping) {
////////////// FIND LINE rotating:
  bool not_used1 = rotate_find_line_sweep(leftThreshLineFollow, rightThreshLineFollow);
  delay(200);
////////
////////////// FOLLOW LINE:
  follow_line_1(leftThreshLineFollow, rightThreshLineFollow, extraThreshLineFollow);
////
////////////////////////////////// FINDING VICTIMS INSIDE CAVE:
/////// CHECK IN FRONT FOR VICTIMS
    bool person_found = sweep_towards_person(); // returns bool of whether person has been found
    if (person_found == true) {
      delay(200);
      bool person_realigned = sweep_realign();
      Serial.println("picking up victim (after delay) ....");
      delayprint(3000);
      pick_up_victim();
      delayprint(200);
      reverse_to_end180();
    
    } else {
      reverse_to_end180();
      locate_victim_side();
      if( turns != 2) {
      bool person_found = sweep_towards_person(); // returns bool of whether person has been found
      if (person_found == true) {
        delay(200);
        bool person_realigned = sweep_realign();
        Serial.println("picking up victim (after delay) ....");
        delayprint(3000);
        pick_up_victim();
        delayprint(200);
        if (turns == 0){
          leftturn();
        } else {
          rightturn();
        }
        reverse_to_end();
      }
      }
    }
//////////////////////////// DROP OFF + BACK TO START
  delay(200);
  forward(1000);
  delay(1000);
  halt();
  rightturn();
  back_to_centre();
  find_line_on_way_back();
  delayprint(1000);
  
  bool not_used2 = rotate_find_line_sweep(leftThreshLineFollow, rightThreshLineFollow);
  delayprint(1000);
  follow_line_2 (leftThreshLineFollow, rightThreshLineFollow, extraThreshLineFollow);
  
  drop_off_victims();
  find_back_of_start_box();

  if (millis()<180000) {
    looping = true;
    forward(200);
    delay(2000);
    halt();
  } else {
    looping = false;
  }
}

  Serial.println(" --------------- END ----------------");
  delayprint(20000);


/////////////////////////////// END  
}


// FUNCTIONS:
////////////////////////////////////////////////////////////////////// MAIN FUNCTIONS:

/// FOLLOW WHITE LINE
void follow_line_1(int leftThresh, int rightThresh, int extraThresh) {
  unsigned long intitial_millis = millis();
  Serial.println("Following the line:");
  char *current_pos_wrt_line = "right_on"; // options: left_off, left_on, on_line, right_on, right_off, on_line.
  Serial.println(current_pos_wrt_line);
  delay(200);
  char *new_pos_wrt_line;
  int defaultSpeed = 100;
  int slowSpeed = 0;
  int new_leftSpeed = defaultSpeed;
  int new_rightSpeed = defaultSpeed;
  int current_leftSpeed = new_leftSpeed;
  int current_rightSpeed = new_rightSpeed;
  forwardvariabledirection(new_leftSpeed, new_rightSpeed);
  int off_count = 0;
  int end_counter = 0;
  bool following_line = true;
  while (following_line) {
    while ((new_leftSpeed == current_leftSpeed) and (new_rightSpeed == current_rightSpeed)) {
      blinking();
      // read the input on analog pin 0 and 1:
      int sensorLeft = analogRead(A2);
      int sensorRight = analogRead(A0);
      int sensorExtra = analogRead(A1);
      // print out the value you read:
      Serial.print("sensorLeft:  ");
      Serial.println(sensorLeft);
      Serial.print("sensorRight:  ");
      Serial.println(sensorRight);
  
     if ((sensorLeft >= leftThresh) && (sensorRight >= rightThresh) && (sensorExtra >= extraThresh) && ((millis() - intitial_millis)>25000)) {
      end_counter +=1;
      if (end_counter > 3) {
        Serial.print("reached end");
        halt();
        following_line = false;
        delayprint(100);
        break;
      }
     } else if ((sensorLeft >= leftThresh) && (sensorRight <= rightThresh)) {
      end_counter = 0;
      new_pos_wrt_line = "right_on";
      new_leftSpeed = defaultSpeed;
      new_rightSpeed = defaultSpeed;
      off_count = 0;
             

      } else if ((sensorLeft >= leftThresh) && (sensorRight >= rightThresh)) {
        end_counter = 0;
        new_pos_wrt_line = "on_line";
        new_leftSpeed = defaultSpeed;
        new_rightSpeed = slowSpeed;
        off_count = 0;
          
      } else if ((sensorLeft <= leftThresh) && (sensorRight >= rightThresh)) {
        end_counter = 0;
        new_pos_wrt_line = "left_on";
        new_leftSpeed = defaultSpeed;
        new_rightSpeed = slowSpeed;
        off_count = 0;
        
      } else if ((sensorLeft <= leftThresh) && (sensorRight <= rightThresh)) {
        end_counter = 0;
        off_count += 1;
        if (off_count >= 20) {
          variableleftturnangle(300);
          bool not_used1 = rotate_find_line_sweep(leftThresh, rightThresh);
          new_pos_wrt_line == "right_on";
          new_leftSpeed = defaultSpeed;
          new_rightSpeed = defaultSpeed;
          off_count = 0;
        } else if ((current_pos_wrt_line == "left_on") || (current_pos_wrt_line == "left_off")) { // Sort of assuming it is going round a right turn, so if both go off at same time, likely that gone off to left.
          new_pos_wrt_line = "left_off";
          new_leftSpeed = defaultSpeed;
          new_rightSpeed = slowSpeed;
        } else if ((current_pos_wrt_line == "right_on") || (current_pos_wrt_line == "right_off") || (current_pos_wrt_line == "on_line")) {
          new_pos_wrt_line =  "right_off";
          new_leftSpeed = slowSpeed;
          new_rightSpeed = defaultSpeed;
        } else if ((current_pos_wrt_line == "on_line")){
          new_pos_wrt_line =  "right_off";
          new_leftSpeed = defaultSpeed;
          new_rightSpeed = defaultSpeed;
          new_pos_wrt_line == "on_line";
        }
      } else {
        end_counter = 0;
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

void follow_line_2(int leftThresh, int rightThresh, int extraThresh) {
  unsigned long intitial_millis = millis();
  Serial.println("Following the line:");
  char *current_pos_wrt_line = "right_on"; // options: left_off, left_on, on_line, right_on, right_off, on_line.
  Serial.println(current_pos_wrt_line);
  delay(200);
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
      blinking();
      // read the input on analog pin 0 and 1:
      int sensorLeft = analogRead(A2);
      int sensorRight = analogRead(A0);
      int sensorExtra = analogRead(A1);
      // print out the value you read:
      Serial.print("sensorLeft:  ");
      Serial.println(sensorLeft);
      Serial.print("sensorRight:  ");
      Serial.println(sensorRight);
  
     if ((sensorLeft >= leftThresh) && (sensorRight >= rightThresh) && (sensorExtra >= extraThresh) && ((millis() - intitial_millis)>15000)) {
      Serial.print("reached end");
      halt();
      following_line = false;
      delayprint(200);
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
          bool not_used1 = rotate_find_line_sweep(leftThresh, rightThresh);
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

bool rotate_find_line_sweep (int leftThresh, int rightThresh) {
  variableleftturnangle(600);
  bool line_found = false;
  while(line_found == false) {
   line_found = rotate_find_line_new(leftThresh, rightThresh);
   if (line_found == true) {
    return line_found;
   }
   Serial.println("left turn thing");
   variableleftturnangle(700);
   delay(500);
  }
}

//bool rotate_find_line_sweep (int leftThresh, int rightThresh) {
//  bool line_found = false;
//  while(line_found == false) {
//   line_found = rotate_find_line_new(leftThresh, rightThresh);
//   if (line_found == true) {
//    return line_found;
//   }
//   Serial.println("left turn thing");
//   variableleftturnangle(500);
//   delay(500);
//  }
//}

bool rotate_find_line_new (int leftThresh, int rightThresh) {
  bool line_found = false;
  unsigned long intitial_millis = millis();
  Serial.println("rotating and looking for line:");
  rightturnspeedset(50);
  int count = 0;
  while ((count <= 6) && ((millis() - intitial_millis)<4000)) { 
    blinking();
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
    if (count >= 5) {
      line_found = true;
    }
    delay(10);
  }
  halt();
  return line_found; 
}

void rotate_find_line (int leftThresh, int rightThresh) {
  variableleftturnangle(500);
  Serial.println("rotating and looking for line:");
  rightturnspeedset(50);
  int count = 0;
  while (count <= 5) { 
    blinking();
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
  blinking();  
  timer_s.tick(); // tick the timer
  int ultrasonic1reading = ultrasonic1.read();
  int ultrasonic2reading = ultrasonic2.read();
  Serial.println(ultrasonic1reading);
  Serial.println(ultrasonic2reading);
  if ((ultrasonic2reading >= 90) && (ultrasonic2reading <= 150) && (turns==0) && (timer_count_s > 30)) {
    Serial.println("Reached tunnel, turning around and searching other side");
    rightturn();
    delay(500);
    rightturn();
    turns += 1;
    forward(80);
   } else if ((ultrasonic1reading < 25) && (turns==1)) {
    Serial.println("Reached back wall, turning back to tunnel");
    rightturn();
    delay(500);
    rightturn();
    turns += 1;
    finished = true;
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
//        delay(250);
        halt();
        delay(500);
        leftturn();
        finished = true;
        Serial.println("Finished");

        
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
  variableleftturnangle(500);
  delayprint(200);
  int sweep_direction_count = 0;
  while ((sweep_direction_count < 4) and (person_found == false)) {
    blinking();
    Serial.print("sweep_direction_count = ");
    Serial.println(sweep_direction_count);
    delayprint(200);
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
    int pre_x1_ultrasound = ultrasonic1.read();
    int pre_x2_ultrasound = ultrasonic1.read();
    int pre_x3_ultrasound = ultrasonic1.read();
    int pre_x4_ultrasound = ultrasonic1.read();
    int pre_x5_ultrasound = ultrasonic1.read();
    int pre_x6_ultrasound = ultrasonic1.read();
    int pre_x7_ultrasound = ultrasonic1.read();
    int pre_x8_ultrasound = ultrasonic1.read();
    int pre_x9_ultrasound = ultrasonic1.read();
    int pre_x10_ultrasound = ultrasonic1.read();
    int pre_x11_ultrasound = ultrasonic1.read();
    int close_ultrasound = 0;
    int close_counter = 0;
    while (time_counter_a < sweep_start_timer + 30) {
      blinking();
      timer_a.tick(); // tick the timer
      current_ultrasound = ultrasonic1.read();
      Serial.println(current_ultrasound);
      if ((abs(pre_x6_ultrasound-current_ultrasound) >= 10) or (abs(pre_x5_ultrasound-current_ultrasound) >= 10)){
        close_counter += 1;
      } else if ((abs(pre_x7_ultrasound-pre_x1_ultrasound) >= 10) or (abs(pre_x6_ultrasound-pre_x1_ultrasound) >= 10)){
        close_counter += 1;
      } else if ((abs(pre_x8_ultrasound-pre_x2_ultrasound) >= 10) or (abs(pre_x7_ultrasound-pre_x2_ultrasound) >= 10)){
        close_counter += 1;
      } else if ((abs(pre_x9_ultrasound-pre_x3_ultrasound) >= 10) or (abs(pre_x8_ultrasound-pre_x3_ultrasound) >= 10)){
        close_counter += 1;
      } else if ((abs(pre_x10_ultrasound-pre_x4_ultrasound) >= 10) or (abs(pre_x9_ultrasound-pre_x4_ultrasound) >= 10)){
        close_counter += 1;        
      } else if ((abs(pre_x11_ultrasound-pre_x5_ultrasound) >= 10) or (abs(pre_x10_ultrasound-pre_x5_ultrasound) >= 10)){
        close_counter += 1;
      }
      if (close_counter >= 4) {
        delay(1100);
        halt();
        Serial.print("Person Located, distance to person (in cm?) = ");
        Serial.println(current_ultrasound);
        Serial.print("Collecting person...");
        delayprint(2000);
        forward(100);
        delayprint(current_ultrasound * 60);  /// <---------------------- CHANGE DISTANCE CALIBRATION FACTOR HERE
        halt();
        person_found = true;
        return true;
        break;
      } else {
        close_counter = 0;
      }
      pre_x11_ultrasound = pre_x9_ultrasound;
      pre_x10_ultrasound = pre_x9_ultrasound;
      pre_x9_ultrasound = pre_x8_ultrasound;
      pre_x8_ultrasound = pre_x7_ultrasound;
      pre_x7_ultrasound = pre_x6_ultrasound;
      pre_x6_ultrasound = pre_x5_ultrasound;
      pre_x5_ultrasound = pre_x4_ultrasound;
      pre_x4_ultrasound = pre_x3_ultrasound;
      pre_x3_ultrasound = pre_x2_ultrasound;
      pre_x2_ultrasound = pre_x1_ultrasound;
      pre_x1_ultrasound = current_ultrasound;
    }
    halt();
    delayprint(100);
  }
  Serial.print("Reached end of sweeping loop");
  delayprint(200);
  return false;
}


bool sweep_realign() {
  bool person_found = false;
  time_counter_a = 0;
  variableleftturnangle(500);
  delayprint(200);
  int sweep_direction_count = 0;
  while ((sweep_direction_count < 4) and (person_found == false)) {
    blinking();
    Serial.print("sweep_direction_count = ");
    Serial.println(sweep_direction_count);
    delayprint(200);
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
    int pre_x1_ultrasound = ultrasonic1.read();
    int pre_x2_ultrasound = ultrasonic1.read();
    int pre_x3_ultrasound = ultrasonic1.read();
    int pre_x4_ultrasound = ultrasonic1.read();
    int pre_x5_ultrasound = ultrasonic1.read();
    int pre_x6_ultrasound = ultrasonic1.read();
    int pre_x7_ultrasound = ultrasonic1.read();
    int pre_x8_ultrasound = ultrasonic1.read();
    int pre_x9_ultrasound = ultrasonic1.read();
    int pre_x10_ultrasound = ultrasonic1.read();
    int pre_x11_ultrasound = ultrasonic1.read();
    int close_ultrasound = 0;
    int close_counter = 0;
    while (time_counter_a < sweep_start_timer + 30) {
      blinking();
      timer_a.tick(); // tick the timer
      current_ultrasound = ultrasonic1.read();
      Serial.println(current_ultrasound);
      if ((abs(pre_x6_ultrasound-current_ultrasound) >= 10) or (abs(pre_x5_ultrasound-current_ultrasound) >= 10)){
        close_counter += 1;
      } else if ((abs(pre_x7_ultrasound-pre_x1_ultrasound) >= 10) or (abs(pre_x6_ultrasound-pre_x1_ultrasound) >= 10)){
        close_counter += 1;
      } else if ((abs(pre_x8_ultrasound-pre_x2_ultrasound) >= 10) or (abs(pre_x7_ultrasound-pre_x2_ultrasound) >= 10)){
        close_counter += 1;
      } else if ((abs(pre_x9_ultrasound-pre_x3_ultrasound) >= 10) or (abs(pre_x8_ultrasound-pre_x3_ultrasound) >= 10)){
        close_counter += 1;
      } else if ((abs(pre_x10_ultrasound-pre_x4_ultrasound) >= 10) or (abs(pre_x9_ultrasound-pre_x4_ultrasound) >= 10)){
        close_counter += 1;        
      } else if ((abs(pre_x11_ultrasound-pre_x5_ultrasound) >= 10) or (abs(pre_x10_ultrasound-pre_x5_ultrasound) >= 10)){
        close_counter += 1;
      }
      if (close_counter >= 4) {
        delay(1100);
        halt();
        Serial.print("Person Located, distance to person (in cm?) = ");
        Serial.println(current_ultrasound);
        Serial.print("Collecting person...");
        person_found = true;
        return true;
        break;
      } else {
        close_counter = 0;
      }
      pre_x11_ultrasound = pre_x9_ultrasound;
      pre_x10_ultrasound = pre_x9_ultrasound;
      pre_x9_ultrasound = pre_x8_ultrasound;
      pre_x8_ultrasound = pre_x7_ultrasound;
      pre_x7_ultrasound = pre_x6_ultrasound;
      pre_x6_ultrasound = pre_x5_ultrasound;
      pre_x5_ultrasound = pre_x4_ultrasound;
      pre_x4_ultrasound = pre_x3_ultrasound;
      pre_x3_ultrasound = pre_x2_ultrasound;
      pre_x2_ultrasound = pre_x1_ultrasound;
      pre_x1_ultrasound = current_ultrasound;
    }
    halt();
    delayprint(100);
  }
  Serial.print("Reached end of sweeping loop");
  delayprint(200);
  return false;  
}


////////////// MISC. NAVIGATION BITS

void reverse_to_end180() {
  rightturn();
  delayprint(500);
  rightturn();
  delayprint(500);
  backward(250);
  delayprint(8000);
  halt();
  delayprint(2000);
}


void reverse_to_end() {
  delayprint(500);
  backward(250);
  delayprint(8000);
  halt();
  delayprint(2000);
}



void back_to_centre() {
  bool go_to_center = true;
  int ultrasound_reading1 = ultrasonic1.read();
  int ultrasound_reading2 = ultrasonic2.read();
  int i;
  
  while (go_to_center) {
    if (i < 5) {
      i += 1;
    } else {
    if (ultrasound_reading2 >= ultrasound_reading1) {
    backward(70);
    side = "left";
  } else {
    forward(70);
    side = "right";
  }
    }
    blinking();
    int ultrasound_reading1 = ultrasonic1.read();
    int ultrasound_reading2 = ultrasonic2.read();
    Serial.println(ultrasound_reading1);
    Serial.println(ultrasound_reading2);
    if (abs(ultrasound_reading1 - ultrasound_reading2) < 5) {
      go_to_center = false;
    }
    delay(5);    
  }
  halt();
  delayprint(500);                  //<------------ get rid of this
  leftturn();
  Serial.println("Found center and facing tunnel");
  delayprint(2000);
}

void find_line_on_way_back() {
  int counter = 0;
  forward(100);
  while(true) {
    blinking();
    int ultrasonic2reading = ultrasonic2.read();
    Serial.println(ultrasonic2reading);
    if ((ultrasonic2reading >= 80) && (ultrasonic2reading <= 120)) {
      counter += 1;
    } else {
      counter = 0;
    }
    if (counter >= 15) {
      halt();
      delay(500);
      break;
    }
  }
}

void find_back_of_start_box() {
  int counter = 0;
  backward(100);
  while(true) {
    blinking();
    int ultrasonic2reading = ultrasonic2.read();
    Serial.println(ultrasonic2reading);
    if ((ultrasonic2reading <= 10) && (ultrasonic2reading >= 0)) {
      counter += 1;
    } else {
      counter = 0;
    }
    if (counter >= 15) {
      halt();
      delay(500);
      break;
    }
  }
}

void find_line_on_way_back1(int leftThresh, int rightThresh, int extraThresh) {
  forward(100);
  while(true) {
    blinking();
    int sensorLeft = analogRead(A2);
    int sensorRight = analogRead(A0);
    int sensorExtra = analogRead(A1);
    if (((sensorLeft >= leftThresh) && (sensorRight >= rightThresh)) or ((sensorLeft >= leftThresh) && (sensorExtra >= extraThresh)) or ((sensorExtra >= extraThresh) && (sensorRight >= rightThresh)) ) {
      halt();
      delayprint(1000);
      forward(100);
      delay(500);
      halt();
      break;
    }
  }
}

void reverse_to_start(int leftThresh, int rightThresh, int extraThresh) {
  backward(100);
  while(true) {
    blinking();
    int sensorLeft = analogRead(A2);
    int sensorRight = analogRead(A0);
    int sensorExtra = analogRead(A1);
    if (((sensorLeft >= leftThresh) && (sensorRight >= rightThresh)) or ((sensorLeft >= leftThresh) && (sensorExtra >= extraThresh)) or ((sensorExtra >= extraThresh) && (sensorRight >= rightThresh)) ) {
      halt();
      delayprint(1000);
      backward(100);
      delay(500);
      halt();
      break;
    }
  }
}

////////////// SCOOP FUNCTIONS

///// PICK UP PEOPLE
void pick_up_victim(){
  Serial.println("picking up person...");
//  GO TOWARDS PERSON AND STOP WITHIN 5CM
  digitalWrite(ledPin, HIGH);
  delay(1000); // WAIT FOR VICTIM IDENTIFICATION CIRCUIT TO WORK
  digitalWrite(ledPin, LOW);
  delay(4000);
  forward(100); // CHANGE SPEED TO LOWEST POSSIBLE THAT STILL MOVES
  delay(1500);
  halt();
  pullMotor -> run(FORWARD);
  pullMotor -> setSpeed(200);
  delay(3000); // CHANGE SO STOPS WHEN CLOSED
  pullMotor -> setSpeed(0);
  people_collected += 1;
  }

void drop_off_victims () {
  delay(1000);
  forward(1000);
  delay(500);
  halt();
  delay(1000);
  rightturn();
  delay(1000);
  backward(80);
  delay(1000);
  halt();
  delayprint(1000);
  pullMotor -> run(BACKWARD);
  pullMotor -> setSpeed(150);
  delay(2000);
  pullMotor -> setSpeed(0);
  delay(1000);
  ejectMotor -> run(BACKWARD);
  ejectMotor -> setSpeed(150);
  delay(2000); // OPEN MORE THAN 90 DEGREES?
  ejectMotor -> setSpeed(0);
  delay(1000);
  ejectMotor -> run(FORWARD);
  ejectMotor -> setSpeed(150);
  delay(2500);
  ejectMotor -> setSpeed(0);
  delay(1000);
  pullMotor -> run(FORWARD);
  pullMotor -> setSpeed(150);
  delay(2500);
  pullMotor -> setSpeed(0);
  delay(1000);
  backward(80);
  delay(2000);
  halt();
  delay(1000);
  leftturn();
  delay(1000);
  backward(200);
  delay(5000);
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
  blink = true;
  Serial.println("forward");
  myMotor1->run(FORWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(speedSet);
  myMotor2->setSpeed(speedSet+4);
}


void forwardvariabledirection(int leftSpeed, int rightSpeed) {
  blink = true;
  Serial.println("forwardvariabledirection");
  myMotor1->run(FORWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(leftSpeed);
  myMotor2->setSpeed(rightSpeed);
}

void backward(int speedSet) {
  blink = true;
  Serial.println("backward");
  myMotor1->run(BACKWARD);
  myMotor2->run(BACKWARD);
  myMotor1->setSpeed(speedSet);
  myMotor2->setSpeed(speedSet+4);
}

void backwardvariabledirection(int leftSpeed, int rightSpeed) {
  blink = true;
  Serial.println("forward");
  myMotor1->run(BACKWARD);
  myMotor2->run(BACKWARD);
  myMotor1->setSpeed(leftSpeed);
  myMotor2->setSpeed(rightSpeed);
}

void halt() {
  blink = false;
  Serial.println("halt");
  myMotor1->setSpeed(0);
  myMotor2->setSpeed(0);
}


void accelerateforward(int speedSet) {
  blink = true;
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
  blink = true;
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
  blink = true;
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
  blink = true;
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
  blink = true;
  Serial.println("leftturn");
  myMotor1->run(BACKWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(160);
  myMotor2->setSpeed(160);
  delay(1450);
  myMotor1->setSpeed(0);
  myMotor2->setSpeed(0);
}

void rightturn() {
  blink = true;
  Serial.println("rightturn");
  myMotor1->run(FORWARD);
  myMotor2->run(BACKWARD);
  myMotor1->setSpeed(160);
  myMotor2->setSpeed(160);
  delay(1450);
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
  Serial.println("variableleftturnangle");
  myMotor1->run(BACKWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(165);
  myMotor2->setSpeed(165);
  delay(delayangle);
  myMotor1->setSpeed(0);
  myMotor2->setSpeed(0);
}

void leftturnspeedset(int speedSet) {
  blink = true;
  Serial.println("leftturn");
  myMotor1->run(BACKWARD);
  myMotor2->run(FORWARD);
  myMotor1->setSpeed(speedSet);
  myMotor2->setSpeed(speedSet);
}

void rightturnspeedset(int speedSet) {
  blink = true;
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
  timer_count_s += 1;
}

bool increment_time_counter(void *) {
  time_counter_a += 1;
  
//  Serial.print("Time in millis: ");
//  Serial.println(millis());
//
  Serial.print("------------------------------------> time_counter = ");
  Serial.println(time_counter_a);

  return true; // repeat? true
}

void read_line_sensors() {
  while(true) {
    blinking();
    int onLeft = analogRead(A2);
    int onRight = analogRead(A0);
    int onExtra = analogRead(A1);
    // print out the value you read:
    Serial.print("sensorLeft:  ");
    Serial.println(onLeft);
    Serial.print("sensorRight:  ");
    Serial.println(onRight);
    Serial.print("sensorExtra:  ");
    Serial.println(onExtra);
    delay(1000);
  }
}

void read_ultrasound_sensors() {
  while(true) {
    blinking();
    int ultrasound_reading1 = ultrasonic1.read();
    int ultrasound_reading2 = ultrasonic2.read();
    // print out the value you read:
    Serial.print("ultrasound_reading1 (front):  ");
    Serial.println(ultrasound_reading1);
    Serial.print("ultrasound_reading2 (back):  ");
    Serial.println(ultrasound_reading2);
    delay(1000);
  }
}

////// AUTOMATIC CALIBRATION:
int automatic_calibration() {
  Serial.println("Automatic Calibration");
  Serial.println("Taking readings...");
  int onLeft = analogRead(A2);
  int onRight = analogRead(A0);
  int onExtra = analogRead(A1);
  delay(100);
  leftturn();
  halt();
  delay(100);
  int offLeft1 = analogRead(A2);
  int offRight1 = analogRead(A0);
  int offExtra1 = analogRead(A1);
  delay(100);
  rightturn();
  halt();
  delay(100);
  rightturn();
  halt();
  delay(100);
  int offLeft2 = analogRead(A2);
  int offRight2 = analogRead(A0);
  int offExtra2 = analogRead(A1);
  delay(100);
  leftturn();
  delay(100);
  Serial.println("... readings taken");
  leftThreshLineFollow = round((offLeft1+offLeft2+onLeft*2.0)/4.0);
  rightThreshLineFollow = round((offRight1+offRight2+onRight*2.0)/4.0);
  extraThreshLineFollow = round((offExtra1+offExtra2+onExtra*2.0)/4.0);
  Serial.println("RESULTS:");
  Serial.println("onLeft, offLeft1, offLeft2, --- onRight, offRight1, offRight2  --- onExtra, offExtra1, offExtra2");
  Serial.println(onLeft);
  Serial.println(offLeft1);
  Serial.println(offLeft2);
  Serial.println(onRight);
  Serial.println(offRight1);
  Serial.println(offRight2);
  Serial.println(onExtra);
  Serial.println(offExtra1);
  Serial.println(offExtra2);
  Serial.println("Calculated potential thresholds:  ");
  Serial.print("Left threshold = ");
  Serial.println(leftThreshLineFollow);
  Serial.print("Right threshold = ");
  Serial.println(rightThreshLineFollow);
  Serial.print("Extra threshold = ");
  Serial.println(extraThreshLineFollow);   
}


void blinking() {
  if (blink) {
    unsigned long currentMillis = millis();

    if ((currentMillis - previousMillis >= interval) && blink) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
  }
}
}
