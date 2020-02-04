int distSensPin = A0; // select correct input pin
int distSens = 0;
int newDistSens = 0;
int i = 0;
int count = 0;
////// possible states are ["before_corner","corner","tunnel","beyond_tunnel"]
//char state = "before_corner";
int state = 1;
int corner_thresh = 200;
int tunnel_thresh = 250;

void setup() {
  Serial.begin(9600);

}

void loop() {
  newDistSens = analogRead(distSensPin); // Read sensor
  Serial.println(newDistSens); 
  Serial.print(state);
  if (state == 1) {
    Serial.println("Before corner!");
    if (newDistSens < corner_thresh) {
      if (count < 5) {
        Serial.println("Count incremented");
        count += 1;
      } else {
      state = 2;
      Serial.println("Corner Reached!");
      count = 0;
    }
    } else {
      Serial.println("State not changed");
    }
  } else if (state == 2) {
    if (newDistSens > tunnel_thresh) {
      if (count < 5) {
        count += 1;
      } else {
        state = 3;
        Serial.println("Tunnel Reached!");
        count = 0;
      }
    } else {
      Serial.println("State not changed");
    }
  } else if (state == 3) {
    if (newDistSens < tunnel_thresh) {
      if (count < 5) {
        count += 1;
      } else {
        state = 4;
        Serial.println("Beyond Tunnel!");
        ////// stop line following
      }
    }
  }
  distSens = newDistSens;
  delay(2000);
}
