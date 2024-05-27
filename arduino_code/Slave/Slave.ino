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

int rd;
int rd_old; // Global variable that stores value from I2C BUS

// ------------------------------
// ------------------------------

void setup() {

  Wire.begin(SLAVE_ADDR);

  Wire.onReceive(receiveEvent);

  Serial.begin(9600);

  motor1.setSpeed(100);  
  motor2.setSpeed(100);  
  motor3.setSpeed(140);  
  motor4.setSpeed(140);  

  Serial.println("Set up done.");
}

void loop() {
  
  if(rd == 0){
    Stop();
  }

  if(rd == 1){
    Left();
  }
  
  if(rd == 2){
    Right();
  }
 

  if(rd == 3){
    Forward();
  }
  
  delay(50);
}


// ------------------------------
// Functions
// ------------------------------

// This function is responsible for receiving and processing the data from I2C BUS

void receiveEvent() {
  int received_value = Wire.read();
  
  if (received_value != -1) {
    rd = received_value; // Update rd only when received_value is not -1
    rd_old = rd; // Update rd_old only when rd is updated
  }
  
  // Optional: handle the case when received_value is -1
  // This can be done by setting rd to a default value or taking other appropriate action
  
  Serial.println(rd);
}



void Right(){

      motor1.run(BACKWARD); //LT
      motor2.run(FORWARD); //PT
      motor3.run(BACKWARD); //PP
      motor4.run(FORWARD); //LP
      
}

void Left(){

      motor1.run(FORWARD); //LT
      motor2.run(BACKWARD); //PT
      motor3.run(FORWARD); //PP
      motor4.run(BACKWARD); //LP
}

void Forward(){

      motor1.run(BACKWARD);
      motor2.run(BACKWARD);
      motor3.run(BACKWARD);
      motor4.run(BACKWARD);
}

void Stop(){
      motor1.run(RELEASE);
      motor2.run(RELEASE);
      motor3.run(RELEASE);
      motor4.run(RELEASE);
}
// TODO: 
// This function is responsible for calculating the steering


