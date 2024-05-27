#include <Wire.h>
#include "VL53L0X.h"

// ------------------------------
// Definition of global constants
// ------------------------------

int dist_array[7];
bool wneka = false;
bool exitingAlcove = false;
int current_direction = 0;
unsigned long alcoveExitTime = 0;

#define FORWARD_TIME_AFTER_ALCOVE 2500

enum CURRENT_DIR{
  STOP,
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_FORWARD,
};

#define SAFE_DIST 250

// XSHUT PINS are responsible for enabling the change of the VL530LX sensor address
#define XSHUT_pin2 2      
#define XSHUT_pin3 3
#define XSHUT_pin4 4
#define XSHUT_pin5 5
#define XSHUT_pin6 6

// Slave arduino address in the I2C BUS
#define SLAVE_ADDR 9

// New unique addersses of VL530LX sensors
#define Sensor2_newAddress 42
#define Sensor3_newAddress 43
#define Sensor4_newAddress 44
#define Sensor5_newAddress 45
#define Sensor6_newAddress 46


// ------------------------------
// Creation of all necesary global instances
// ------------------------------

VL53L0X Sensor1;
VL53L0X Sensor2;
VL53L0X Sensor3;
VL53L0X Sensor4;
VL53L0X Sensor5;
VL53L0X Sensor6;

// ------------------------------
// ------------------------------

void setup() {

// According to documentation, to change the address, at first the pin must be set to output
pinMode(XSHUT_pin2, OUTPUT);
pinMode(XSHUT_pin3, OUTPUT);
pinMode(XSHUT_pin4, OUTPUT);
pinMode(XSHUT_pin5, OUTPUT);
pinMode(XSHUT_pin6, OUTPUT);

Wire.begin();
Serial.begin(9600);


// To actually change the address of VL530LX sensor the setAddress() method should be used
// Then the XSHUT pin must be set to input and delayed for 10ms.
Sensor2.setAddress(Sensor2_newAddress);
pinMode(XSHUT_pin2, INPUT);
delay(10);

Sensor3.setAddress(Sensor3_newAddress);
pinMode(XSHUT_pin3, INPUT);
delay(10);

Sensor4.setAddress(Sensor4_newAddress);
pinMode(XSHUT_pin4, INPUT);
delay(10);

Sensor5.setAddress(Sensor5_newAddress);
pinMode(XSHUT_pin5, INPUT);
delay(10);

Sensor6.setAddress(Sensor6_newAddress);
pinMode(XSHUT_pin6, INPUT);
delay(10);

// Conditional statements used for debug to check if any error occured durgin initialization

if(Sensor1.init()){
  Serial.print('Sensor 1 initialized properly');
}
if(Sensor2.init()){
  Serial.print('Sensor 2 initialized properly');
}

if(Sensor3.init()){
  Serial.print('Sensor 3 initialized properly');
}
if(Sensor4.init()){
  Serial.print('Sensor 4 initialized properly');
}
if(Sensor5.init()){
  Serial.print('Sensor 5 initialized properly');
}

if(Sensor6.init()){
  Serial.print('Sensor 6 initialized properly');
}


// In order to omit infinite read from the sensor the timeout should be set
// If the timeout occurs the sensor return value = MAX(uint16)
Sensor1.setTimeout(500);
Sensor2.setTimeout(500);
Sensor3.setTimeout(500);
Sensor4.setTimeout(500);
Sensor5.setTimeout(500);
Sensor6.setTimeout(500);


// Set sensors to continous read
Sensor1.startContinuous();
Sensor2.startContinuous();
Sensor3.startContinuous();
Sensor4.startContinuous();
Sensor5.startContinuous();
Sensor6.startContinuous();

Serial.println("Master initialized properly.");
}

// ------------------------------
// ------------------------------

void loop() {
  searchForDevices();

  readFromSensor();

  processSensorInfo();

  sendSteering();

  delay(100);
}


// ------------------------------
// Functions
// ------------------------------

// This function is used to search for discovering accessible devices on the I2C BUS
void searchForDevices() {
  byte error, address;
  int nDevices;
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("done");
}

// This function is used to read data from sensors
void readFromSensor() {
  // Serial.print("Odczyt z pierwszego czujnika: ");
  // Serial.println(Sensor1.readRangeContinuousMillimeters());
  dist_array[0] = Sensor1.readRangeContinuousMillimeters();

  // Serial.print("Odczyt z drugiego czujnika: ");
  // Serial.println(Sensor2.readRangeContinuousMillimeters());
  dist_array[1] = Sensor2.readRangeContinuousMillimeters();

  // Serial.print("Odczyt z 3 czujnika: ");
  // Serial.println(Sensor3.readRangeContinuousMillimeters());
  dist_array[2] = Sensor3.readRangeContinuousMillimeters();

  // Serial.print("Odczyt z 4 czujnika: ");
  // Serial.println(Sensor4.readRangeContinuousMillimeters());
  dist_array[3] = Sensor4.readRangeContinuousMillimeters();

  // Serial.print("Odczyt z 5 czujnika: ");
  // Serial.println(Sensor5.readRangeContinuousMillimeters());
  dist_array[4] = Sensor5.readRangeContinuousMillimeters();

  // Serial.print("Odczyt z 6 czujnika: ");
  // Serial.println(Sensor6.readRangeContinuousMillimeters());
  dist_array[5] = Sensor6.readRangeContinuousMillimeters();
}

// This function will be used to process and prepare data to be sent to slave
int processSensorInfo() {
  unsigned long currentTime = millis();

  // Tryb normalny
  if (!wneka) {
    // Jeśli robot znajduje się w otwartej przestrzeni, porusza się w prawo
    if ((dist_array[4] > SAFE_DIST && dist_array[4] > SAFE_DIST) && 
        (dist_array[2] > SAFE_DIST && dist_array[1] > SAFE_DIST)) {
      Serial.println("ENGINES TO RIGHT");
      current_direction = CURRENT_DIR::MOVE_RIGHT;
      return CURRENT_DIR::MOVE_RIGHT;
    }

    // Jeśli robot napotyka przeszkodę z przodu, wchodzi w tryb wnęki
    if ((dist_array[4] <= SAFE_DIST) || (dist_array[3] <= SAFE_DIST)) {
      current_direction = CURRENT_DIR::MOVE_LEFT;
      Serial.println("ENGINES TO LEFT");
      wneka = true;
      return CURRENT_DIR::MOVE_LEFT;
    }

    // Jeśli przed robotem nie ma przeszkód, porusza się do przodu
    if (dist_array[4] > SAFE_DIST && dist_array[3] > SAFE_DIST) {
      Serial.println("ENGINES FORWARD");
      current_direction = CURRENT_DIR::MOVE_FORWARD;
      return CURRENT_DIR::MOVE_FORWARD;
    }

    // Jeśli odległość od ściany jest większa niż bezpieczna, porusza się w prawo
    if ((current_direction != CURRENT_DIR::MOVE_LEFT) && dist_array[2] > SAFE_DIST && dist_array[1] > SAFE_DIST) {
      Serial.println("ENGINES TO RIGHT");
      current_direction = CURRENT_DIR::MOVE_RIGHT;
      return CURRENT_DIR::MOVE_RIGHT;
    }
  }

  // Tryb wnęki
  if (wneka) {
    // Jeśli przed robotem jest przeszkoda, kontynuuje ruch w lewo
    if ((dist_array[4] <= SAFE_DIST) || (dist_array[3] <= SAFE_DIST)) {
      Serial.println("ENGINES TO LEFT - WNEKA");
      current_direction = CURRENT_DIR::MOVE_LEFT;
      return CURRENT_DIR::MOVE_LEFT;
    }

    // Warunek wyjechania z wnęki - jeśli robot ma bezpieczną odległość z przodu i z boku
    if(!exitingAlcove && dist_array[4] > SAFE_DIST && dist_array[3] > SAFE_DIST) {
      Serial.println("EXITING ALCOVE, SWITCHING TO FORWARD MODE");
      exitingAlcove = true;
      alcoveExitTime = currentTime;
      current_direction = CURRENT_DIR::MOVE_FORWARD;
      return CURRENT_DIR::MOVE_FORWARD;
    }
  }

  // Po wyjechaniu z wnęki, jedź prosto przez określony czas
  if (exitingAlcove) {
    if (currentTime - alcoveExitTime < FORWARD_TIME_AFTER_ALCOVE) {
      Serial.println("ENGINES FORWARD AFTER ALCOVE");
      current_direction = CURRENT_DIR::MOVE_FORWARD;
      return CURRENT_DIR::MOVE_FORWARD;
    } else {
      Serial.println("RESUMING NORMAL MODE");
      wneka = false;
      exitingAlcove = false;
    }
  }
}

// This function is a very basic example of sending value to slave via I2C BUS
void sendSteering() {
  Wire.beginTransmission(SLAVE_ADDR);
  int temp = processSensorInfo();
  Serial.println(temp);
  Wire.write(temp);
  Wire.endTransmission();
}
