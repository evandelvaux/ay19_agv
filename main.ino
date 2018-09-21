/*
  Main 

 */
#include <Arduino.h>
#include <Servo.h>
#include <Sabertooth.h>
#define LIGHT_THRESH 350
#define LEFT_SIDE 1 // 1 = follow left wall, 0 = follow right wall


Servo servoLeft;  // create servo object to control a servo 
Servo servoRight;

int timer = 100;           // The higher the number, the slower the timing.

int pingRightOutput = A0;
int pingLeftOutput = A1;
int prFrontInput = A2;
int prTopInput = A3;

int irRightOutput = 2;
int irRightInput = 3;
int irFRightOutput = 4;
int irFRightInput = 5;
int irFrontOutput = 6;
int irFrontInput = 7;
int irDownOutput = 8;
int irDownInput = 9;
int irFLeftOutput = 10;
int irFLeftInput = 11;
int irLeftOutput = A4;
int irLeftInput = A5;

void setup() {

  servoLeft.attach(13);
  servoRight.attach(12);

  pinMode(pingRightOutput, OUTPUT); // pingRight
  pinMode(pingLeftOutput, OUTPUT); // pingLeft
  pinMode(prFrontInput, INPUT); // prFront
  pinMode(prTopInput, INPUT); // prTop

  pinMode(irRightOutput, OUTPUT); // for irRight
  pinMode(irRightInput, INPUT); // for irRight
  pinMode(irFRightOutput, OUTPUT); // for irFRight
  pinMode(irFRightInput, INPUT); // for irFRight
  pinMode(irFrontOutput, OUTPUT); // for irFront
  pinMode(irFrontInput, INPUT); // for irFront
  pinMode(irDownOutput, OUTPUT); // for irDown
  pinMode(irDownInput, INPUT);  // for irDown
  pinMode(irFLeftOutput, OUTPUT); // for irFLeft
  pinMode(irFLeftInput, INPUT); // for irFLeft
  pinMode(irLeftOutput, OUTPUT); // for irLeft
  pinMode(irLeftInput, INPUT); // for irLeft
  Serial.begin(9600);
}

void loop() {
  // returns 1 or 0 depending on whether an object was detected
  int irRight = irDetect(irRightOutput, irRightInput, 38000);
  int irFRight = irDetect(irFRightOutput, irFRightInput, 38000);
  int irFront = irDetect(irFrontOutput, irFrontInput, 38000);
  int irDown = irDetect(irDownOutput, irDownInput, 38000);
  int irFLeft = irDetect(irFLeftOutput, irFLeftInput, 38000);
  int irLeft = irDetect(irLeftOutput, irLeftInput, 38000);

  //moveFwd(30); // we like this distance, will put at the end of code after sensors run
  //moveBack(4);
  //turnLeft(30);
  //turnRight(20);
  //darkRoomLoop();
  
  stop(3);
  
  //servoLeft.writeMicroseconds(1500);    // motor stop 
  //servoRight.writeMicroseconds(1500);
  // returns a cm distance
  int pingRight = pingDetect(pingRightOutput);
  int pingLeft = pingDetect(pingLeftOutput);
  
  // returns a value for how much light is reserved
  int prFront = prDetect(prFrontInput);
  int prTop = prDetect(prTopInput);
 // Serial.println(prFront);
  checkTransition(prTop);
  detectObstacle(irFront, irDown, irFLeft, irFRight, pingRight, pingLeft, irRight, irLeft);
    /* Must detect both physical & virtual objects. Front left/right used to
      decide which way to best navigate the obstacle. */
//  if (detectOpening(pingLeft, irLeft)) { // Look for opening to the LEFT
//    turnLeft(10);
//    // and resume moving forward behavior.
//  }
//  else { // the else statement is so that when it turns that direction, it doesn't try to turn again
//  // to go down the long hallway.
//      if (detectOpening(pingRight, irRight)) {// Look for opening to the RIGHT
//        turnRight(10);
//        // and continue forward movement behavior.
//        }
//    }
//
  // Adjust duration parameter as testing dictates
  //stop(3);
  moveFwd(30);
  Serial.println("Moving forward as normal.");
//  // delay(15); // If needed
//  servoLeft.writeMicroseconds(1500);    // motor stop 
//  servoRight.writeMicroseconds(1500);
}


/* ---------------------------------
            SERVO HELPERS           */
void turnLeft (int dur) {
    int i;
    for (i = 0; i < dur; i++) {
        servoLeft.writeMicroseconds(2000);
        servoRight.writeMicroseconds(1000);
        delay(15);
    } 
}
void turnRight (int dur) {
    int i;
    for (i = 0; i < dur; i++) {
        servoLeft.writeMicroseconds(1000);
        servoRight.writeMicroseconds(2000);
        delay(15);
    }   
}
void moveFwd (int dur) {
    int i;
    for (i = 0; i < dur; i++) {
        servoLeft.writeMicroseconds(1250);
        servoRight.writeMicroseconds(1250);
        delay(15);
    } 
}
void moveBack (int dur) {
    int i;
    for (i = 0; i < dur; i++) {
        servoLeft.writeMicroseconds(2000);
        servoRight.writeMicroseconds(2000);
        delay(15);
    } 
}
void stop(int dur) {
    int i;
    for (i = 0; i < dur; i++ ) { 
        servoLeft.writeMicroseconds(1500);    // motor stop 
        servoRight.writeMicroseconds(1500);
        delay(15); 
    }
}


/* ---------------------------------
            CONVERSION HELPERS           */
long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}

/* ---------------------------------
            SENSOR HELPERS           */

// IR Object Detection Function
//               OUTPUT        INPUT
int irDetect(int irLedPin, int irReceiverPin, long frequency)
{
  tone(irLedPin, frequency, 8);              // IRLED 38 kHz for at least 1 ms
  delay(1);                                  // Wait 1 ms
  int ir = digitalRead(irReceiverPin);       // IR receiver -> ir variable
  delay(1);                                  // Down time before recheck
  return ir;                                 // Return 1 no detect, 0 detect
}

int pingDetect(int pingPin) {
  // establish variables for duration of the ping, and the distance result
  // in inches and centimeters:
  delay(100);
  long duration, inches, cm;
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(5);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  // convert the time into a distance
  //inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  return cm; 
  // delay(100); I moved this to the beginning of the function so that the data returned is for the
  // most up to date data.
}

int prDetect(int LDR) {
  delay(500);
  int v = analogRead(LDR);
  return v;  
}


/* ---------------------------------
            DETECTION & BEHAVIOR           */

void detectObstacle(int irFront, int irDown, int irFLeft, int irFRight, int pingRight, int pingLeft, int irRight, int irLeft) {
    if (irFront == 0) {
        // This means that there is a wall ahead of it because it goes from not reflecting anything 0 to reflecting 1.
        // there should also be 40 cm of space to work with because that is about how far out you have to be to see the wall with the sensor.
        Serial.println("Something in front detected. Stopping for 2.");
        stop(2); // stops the arduino from hitting it. (hopefully)
        if (irFLeft == 0) {
            // we would need to turn right
            Serial.println("There is something detected in the front left.");
            if (irFRight == 0) {
                Serial.println("No space to turn right, something detected to the front right");
                // which means there is no space to turn right.
                // and we need to check which direction to turn to. 
                // so, unless we bring the side pings in, it'll just guess to a direction? lets say left for right now.          
                // UPDATE: I brought in the pings :D
                if (pingRight <= pingLeft) { // do not forget to add in for the virtual walls     
                    // this means that there is more space (or equal) to the Right so head that way.
                    Serial.println("There is the most space over to the right, so I am turning that direction.");
                    moveBack(20);
                    turnLeft(20);
                    moveFwd(30);
                    turnRight(10);
                }
                else {
                    Serial.println("There is the most space over to the left so I am turning that direction.");
                    // just go the other way, so to the left.
                    moveBack(20);
                    turnRight(20);
                    moveFwd(30);
                    turnLeft(10);
                }
                // and resume normal moving behavior.
            } 
            else {
                Serial.println("Something right! I'm turning left!");
                // and there is space left
                turnRight(20);
                moveFwd(30);
                turnLeft(10);
                // and thus resume normal moving behavior.
            }
        }
        else if (irFRight == 0) {
            // we would need to turn left
            Serial.println("There is something to the front right, we need to turn left.");
            if (irFLeft == 1) {
              Serial.println("There is space to the left! Yay, turn left.");
                // and there is space to the left
                turnLeft(20);
                moveFwd(30);
                turnRight(20);
                // return to normal movement behavior.
            }
            else {
                // the current behavior for if there isn't space immediately to turn.
                Serial.println("There is no space to the left so wont be able to turn.");
                if (pingRight <= pingLeft) {
                  Serial.println("There is more space to the right so turn in that direction after backing up.");
                    // this means that there is more space (or equal) to the Right so head that way.
                    moveBack(20);
                    turnRight(20);
                    moveFwd(30);
                    turnLeft(20);
                }
                else {
                    // just go the other way, so to the left.
                    Serial.println("There is more space to the left so turn in that direction after backing up.");
                    moveBack(20);
                    turnLeft(20);
                    moveFwd(30);
                    turnLeft(20);
                }
                // and thus resume normal moving behavior.
            }
        }
    }
    else if (irDown == 1) {
        // This also signifies if there is a virtual wall ahead which is less than 10 cm out.
        stop(2);
        Serial.println(irDown);
        Serial.println("Saw a virtual wall in front");
        // This is the same procedure for if there is no space to either side. 
        moveBack(30);
        turnLeft(10);
       // moveFwd(30);
        //turnRight(20);
    }
    else {
        if (LEFT_SIDE == 1) {
            checkWall(pingLeft);
        }
        else {
            checkWall(pingRight);
        }
    }
    Serial.println("Resume normal behavior.");
}

void checkWall(int pingSide) {
    int tooClose = 20;
    int tooFar = 50;
    // Behavior determined by whether following left or right wall
    // To change, modify "#define LEFT_SIDE" at top of file
    if (LEFT_SIDE == 1) {
        if (pingSide <= tooClose) {
            Serial.print("Too close to wall (");
            Serial.print(pingSide);
            Serial.println(") , correct by turning right.");
            turnRight(10);
        }
        else if (pingSide > tooFar) {
            Serial.print("Too close to wall (");
            Serial.print(pingSide);
            Serial.println(") , correct by turning left.");
            turnLeft(10);
        }
    }
    else {
        if (pingSide <= tooClose) {
            Serial.print("Too close to wall (");
            Serial.print(pingSide);
            Serial.println(") , correct by turning left.");
            turnLeft(10);
        }
        else if (pingSide > tooFar) {
            Serial.print("Too close to wall (");
            Serial.print(pingSide);
            Serial.println(") , correct by turning right.");
            turnRight(10);
        }
    }

}

boolean detectOpening(int ping, int ir) { // Can be used for either direction
    return (ping >= 30 && ir == 0);
    // says that the opening has to be more than 30 cm away, ie the next wall is more than 30cm. I can bring this down to 20 easily.
    // yay there is an opening! we will need to turn in that direction so I'm going to put it as a boolean
}


/* ---------------------------------
            DARK ROOM BEHAVIOR           */

// Check if need to transition to dark room behavior
void checkTransition (int pr) {
    Serial.print("This is the value of the light from prTop : ");
    Serial.println(pr);
    if (pr < LIGHT_THRESH) {
        darkRoomLoop();
    }
    //return;
}



// Main loop for dark room behavior
int darkRoomLoop (void) {
    Serial.println("Starts dark room loop");
    const int dur = 1;
    const int turns = 12; // # of turns to do
    const int allowedError = 50; // How far below light level will be the minimum
        /* TODO: optimize allowedError. Too low = might not reach that light level.
            Too high = won't be precise in locating the light */
    int i;

    int lightLevelMax = prDetect(prFrontInput);
    int lightLevel;
    
    // Determine highest light level
    turnRight(dur*40);
    for (i=0; i < turns; i++) {
        turnLeft(dur);
        //stop(1);
        int lightLevel = prDetect(prFrontInput);
        Serial.print("Light level: ");
        Serial.println(lightLevel);
        if (lightLevel > lightLevelMax) {
            lightLevelMax = lightLevel;
            Serial.println("  (new max)");
        }
    }
    Serial.print("Final max value of ");
    Serial.print(lightLevelMax);
    Serial.println(". Returning to max...");

    // Return to max light level
    while (lightLevel < (lightLevelMax + allowedError)) {
        turnRight(dur*20);
        //stop(1);
        int lightLevel = prDetect(prFrontInput);
        Serial.print("Light level: ");
        Serial.println(lightLevel);
        if (lightLevel > (lightLevelMax-allowedError)) {
          break;
        }
    }
    Serial.println("Max value reached. Beginning to move...");

    // Keep going forward
    while (true) {
      Serial.println("Moving forward");
      moveFwd(dur);
    }

} // end darkRoomLoop



int darkRoomLoopOld (void) {
    Serial.println("Starts dark room loop");
    const int dur = 1;
    int i;
    // Initial behavior: get bearings
    int lightLevelPrev = prDetect(prFrontInput);
 //   turnLeft(dur);
    int lightLevel = prDetect(prFrontInput);
    
    // Compare light readings
    if (lightLevelPrev < lightLevel) {
      Serial.println("light level increased so keep going left");
        // Light level increased: keep going left
        while (lightLevelPrev < lightLevel) {
            lightLevelPrev = lightLevel;
            turnLeft(dur);
            lightLevel = prDetect(prFrontInput);
        }
        // Light level maxed out, now decreasing
        // Turn back the other way
 //       turnRight(dur);
    }
    else {
      Serial.println("light level decreased turn right.");
        // Light level decreased: turn right
        turnRight(dur*2);
        lightLevel = prDetect(prFrontInput);
        while (lightLevelPrev < lightLevel) {
            lightLevelPrev = lightLevel;
            turnRight(dur);
            lightLevel = prDetect(prFrontInput);
        }
        // Light level maxed out, now decreasing
        // Turn back the other way
  //      turnLeft(dur);
    }

    // Keep going forward
    while (true) {
      Serial.println("Moving forward");
      moveFwd(dur);
    }

} // end darkRoomLoop