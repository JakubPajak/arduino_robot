#include <Wire.h>
#include <AFMotor.h>

#define SLAVE_ADDR 9            // Address of the salve device
#define THRESHOLD_DISTANCE 20   // borderline for distance to the wall

// ------------------------------
// Creation of all necesary global instances
// ------------------------------

// Motor drivers
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

// ------------------------------
// Other global variables
// ------------------------------

int rd; // Global variable that stores value from I2C BUS

// ------------------------------
// ------------------------------

void setup() {

  Wire.begin(SLAVE_ADDR);

  Wire.onReceive(receiveEvent);

  Serial.begin(9600);

  motor1.setSpeed(250);  
  motor2.setSpeed(250);  
  motor3.setSpeed(250);  
  motor4.setSpeed(250);  

  Serial.println("Set up done.");
}

void loop() {

  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}


// ------------------------------
// Functions
// ------------------------------

// This function is responsible for receiving and processing the data from I2C BUS
void receiveEvent() {
  rd = Wire.read();
  if (rd >= 0) {
    Serial.println(rd);
    if (rd < THRESHOLD_DISTANCE) {
      motor1.run(FORWARD);
      motor2.run(FORWARD);
      motor3.run(FORWARD);
      motor4.run(FORWARD);
    } else {
      motor1.run(RELEASE);
      motor2.run(RELEASE);
      motor3.run(RELEASE);
      motor4.run(RELEASE);
    }
  }
}

// TODO: 
// This function is responsible for calculating the steering


