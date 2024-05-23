#include <ArduinoBLE.h>

const char *deviceServiceUuid = "19b10000-e8f2-537e-4f6c-d104768a1214";
const char *deviceServiceRequestCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1215";
const char *deviceServiceResponseCharacteristicUuid = "19b10001-e8f2-537e-4f6c-d104768a1216";

BLEService servoService(deviceServiceUuid);
BLEStringCharacteristic servoRequestCharacteristic(
  deviceServiceRequestCharacteristicUuid, BLEWrite, 20); // Increased buffer size to 20
BLEStringCharacteristic servoResponseCharacteristic(
  deviceServiceResponseCharacteristicUuid, BLENotify, 20); // Increased buffer size to 20

void t5Callback(BLEDevice central, BLECharacteristic characteristic) {
  if (central.connected())
  {
    if (characteristic == servoRequestCharacteristic)
    {
      int16_t value = 0;
      servoRequestCharacteristic.readValue(&value, 2);
      Serial.println("Received value: ");
      Serial.println(value);
    }
  }
}

void setup() {
  Serial.begin(9600);

  BLE.setDeviceName("Arduino APP");
  BLE.setLocalName("Arduino APP");

  if (!BLE.begin()) {
    Serial.println("- Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  BLE.setAdvertisedService(servoService);
  servoService.addCharacteristic(servoRequestCharacteristic);
  servoService.addCharacteristic(servoResponseCharacteristic);
  servoRequestCharacteristic.setEventHandler(BLEWritten, t5Callback); // Set the callback function for when data is written to the characteristic
  BLE.addService(servoService);
  servoResponseCharacteristic.writeValue("0");

  BLE.advertise();

  Serial.println("Arduino BLE (Peripheral Device)");
  Serial.println(" ");
}

void loop() {
  BLEDevice central = BLE.central();
  Serial.println("- Discovering central device...");
  delay(1000);

  if (central) {
    Serial.println("* Connected to central device!");
    Serial.print("* Device MAC address: ");
    Serial.println(central.address());
    Serial.println(" ");

    while (central.connected()) {
      if (servoRequestCharacteristic.written()) {
        int receivedValue = servoRequestCharacteristic.value().toInt();
        // Serial.print("Received value: ");
        // Serial.println(receivedValue);
      }
    }
    Serial.println("* Disconnected from central device!");
  }
}
