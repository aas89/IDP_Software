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

#include <Ultrasonic.h>

Ultrasonic ultrasonic1(10, 11); // 10=yellow, 11=green
Ultrasonic ultrasonic2(12, 13); // 12=yellow, 13=black
//int distance1;                        /// for adding where needed
//int distance2;
//distance1 = ultrasonic1.read();
//distance2 = ultrasonic2.read();

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
}

/////// GLOBAL THRESHOLDS:
  int leftThreshLineFollow;
  int rightThreshLineFollow;

  

// the loop routine runs over and over again forever:
void loop() {
  printHelloWorld();
  delayprint(6000);
 
//////// INITIAL TEST OF SENSORS:
//  read_line_sensors();

//////////////////////////// NAVIGATION:
  sweep_towards_person();
  delayprint(1000);


//// END
//  Serial.println(" -- END");
  delayprint(1000);
};


// FUNCTIONS:
////////////////////////////////////////////////////////////////////// MAIN FUNCTIONS:

void sweep_towards_person() {
  bool sweeping = true;
  int sweep_direction_count = 0;
  int sweep = false;
  bool sweep1 = false;
  bool sweep2 = false;
  while (sweeping) {
    //// start sweep:
    if (sweep_direction_count == 0) {
      forwardvariabledirection(70, 0);
      sweep_direction_count = 1;
    } else if (sweep_direction_count == 1) {
      forwardvariabledirection(0, 100);
      sweep_direction_count = 0;
    } else {
      Serial.println("error with sweep_direction_count");
    }
    
    if ((sweep1 == true) and (sweep2 == true)) {
      Serial.println("BOTH SWEEPS ARE TRUE ----> PERSON DETECTED");
      delayprint(3000);
      sweep_locating();
      Serial.println("POINTING IN CORRECT DIRECTION???");
      delayprint(6000);
      /// function to go towards victim and check distance is decreasing
    } else if ((sweep1 == true) and (sweep2 == false)) {
      Serial.println("SECOND SWEEP i.e. double checking");
      delayprint(3000);
      sweep = checking_sweep();
      sweep1 = sweep;
      sweep2 = sweep;      
    } else if ((sweep1 == false) and (sweep2 == false)) {
      Serial.println("FIRST SWEEP i.e. looking for person");
      sweep = checking_sweep();
      sweep1 = sweep;
      sweep2 = false;
    } else {
      Serial.println("reached else that should not get to so somehow (sweep1 == false and sweep2 == true)");
    }
  }
}


bool checking_sweep() {
  int initial_ultrasound = ultrasonic1.read();
  Serial.print("initial_ultrasound = ");
  Serial.println(initial_ultrasound);
  int ultrasound_reading;
  int middle_ultrasound;
  int first_far = true;
  int middle_close = false;
  int second_far = false;
  int sweep_success = false;
  int sweep_loop_count = 0;
  int middle_counter = 0;
  int scnd_far_counter = 0;
  bool wall_detect = true;
  while (sweep_loop_count < 100) {
    ultrasound_reading = ultrasonic1.read();
    if (ultrasound_reading > 20) {
      wall_detect = false;
    }
    if ((first_far == true) and (middle_close == false) and (second_far == false)) {
      Serial.println("checking for middle close...");
      if (ultrasound_reading <= (initial_ultrasound - 20)) {
        middle_counter += 1;
        scnd_far_counter = 0;
        if (middle_counter > 5) {
          Serial.println("MIDDLE_CLOSE = TRUE");
          middle_close = true;
          middle_ultrasound = ultrasound_reading;
          Serial.print("middle_ultrasound = ");
          Serial.println(middle_ultrasound);}        
      } else {
        middle_counter = 0;
        scnd_far_counter = 0;}
    } else if ((first_far == true) and (middle_close == true) and (second_far == false)) {
      Serial.print("middle_ultrasound + 20 = ");
      Serial.println((middle_ultrasound + 20));
      Serial.print("ultrasound_reading = ");
      Serial.println(ultrasound_reading);
//      Serial.println("checking for second far...");
      if (ultrasound_reading >= (middle_ultrasound + 20)) {
        Serial.println("second far threshold met");
        scnd_far_counter += 1;
        if (scnd_far_counter > 5) {
          Serial.println("SECOND_FAR = TRUE");
          second_far == true;
          Serial.print("ultrasound_reading = ");
          Serial.println(ultrasound_reading);}
      } else {
        middle_counter = 0;
        scnd_far_counter = 0;}
    } else if ((first_far == true) and (middle_close == true) and (second_far == true)) {
      Serial.println("SWEEP_SUCCESS = TRUE");
      sweep_success = true;
    } else {
      Serial.println("reached unreachable else in sweep loop function");
    }
    sweep_loop_count += 1;
    if (round(sweep_loop_count/10.0) == 0) {
      Serial.print("sweep_loop_count = ");
      Serial.println(sweep_loop_count);
    }
    delay(50);     
  }
  if (wall_detect == true) {
    halt();
    Serial.println("WALL DETECTED");
    delayprint(4000);
  }
  Serial.println("--------------------------------LOOP ENDED");
  halt();
  return sweep_success;
}

void sweep_locating() {
  int initial_ultrasound = ultrasonic1.read();
  int ultrasound_reading;
  int detect_counter = 0;
  while (true) {
    ultrasound_reading = ultrasonic1.read();
    if (ultrasound_reading <= (initial_ultrasound - 20)) {
      detect_counter += 1;
      if (detect_counter > 5) {
        break;       
    } else {
      detect_counter = 0;}
  }
  halt();
}
  
}
//////////////////////////////////////////////////////////////////////// SEQUENCES:


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

void read_line_sensors() {
    int sensorLeft = analogRead(A0);
    int sensorRight = analogRead(A3);
    // print out the value you read:
    Serial.print("sensorLeft:  ");
    Serial.println(sensorLeft);
    Serial.print("sensorRight:  ");
    Serial.println(sensorRight);
}