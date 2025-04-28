#include <AccelStepper.h> // You can leave this include for later if needed

// Motor 1 pins (manual control)
const int dirPin1 = 2;
const int stepPin1 = 3;

/*
// Motor 2 pins (AccelStepper - not used right now)
const int dirPin2 = 4;
const int stepPin2 = 5;

// Define AccelStepper motor interface type
#define motorInterfaceType 1 // 1 = Driver (step+dir)

// Create AccelStepper instance for Motor 2
AccelStepper motor2(motorInterfaceType, stepPin2, dirPin2);
*/

const int stepsPerRevolution = 200; // 360 degrees = 200 steps

void setup() {
  // Motor 1 setup (manual)
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);

  /*
  // Motor 2 setup (AccelStepper) - not used for now
  motor2.setMaxSpeed(1000); 
  motor2.setAcceleration(500); 
  */
}

void loop() {
  // ========== Motor 1: 180 degrees (MANUAL) ==========

  // Set Motor 1 direction
  digitalWrite(dirPin1, HIGH);

  // Move Motor 1: 180 degrees = 100 steps manually
  for (int i = 0; i < stepsPerRevolution / 2; i++) {
    digitalWrite(stepPin1, HIGH);
    delayMicroseconds(2000);  // Speed control
    digitalWrite(stepPin1, LOW);
    delayMicroseconds(2000);
  }

  delay(1000); // Wait 1 second before spinning again

  /*
  // ========== Motor 2: 90 degrees out and back (AccelStepper) - not active now ==========

  int steps90deg = stepsPerRevolution / 4; // 90 degrees = 50 steps

  // Move forward 90 degrees
  motor2.move(steps90deg);
  motor2.runToPosition();

  delay(500); // Pause

  // Move backward 90 degrees
  motor2.move(-steps90deg);
  motor2.runToPosition();
  */
}