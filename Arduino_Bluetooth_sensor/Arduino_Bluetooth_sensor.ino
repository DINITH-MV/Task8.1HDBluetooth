/*
  Battery Monitor

  This example creates a Bluetooth® Low Energy peripheral with the standard battery service and
  level characteristic. The A0 pin is used to calculate the battery level.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  You can use a generic Bluetooth® Low Energy central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;

BLEService sensorService("180C");

// Bluetooth® Low Energy Battery Level Characteristic
BLEUnsignedCharCharacteristic lightLevelChar("1A79",  // standard 16-bit characteristic UUID
                                             BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(LED_BUILTIN, OUTPUT);

  Wire.begin();
  lightMeter.begin();

  Serial.println(F("BH1750 begin"));

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("distanceMonitor");
  BLE.setAdvertisedService(sensorService);           // add the service UUID
  sensorService.addCharacteristic(lightLevelChar);  // add the battery level characteristic
  BLE.addService(sensorService);                     // Add the battery service
  lightLevelChar.writeValue(0);                      // set initial value for this characteristic

  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {
      long currentMillis = millis();
      // if 200 ms have passed, check the battery level:
      float lux = lightMeter.readLightLevel();
      lightLevelChar.writeValue(lux);
      delay(200);
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}