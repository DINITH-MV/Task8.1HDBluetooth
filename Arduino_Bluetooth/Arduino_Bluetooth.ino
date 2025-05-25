#include <ArduinoBLE.h>

// → Ultrasonic pins
const int trigPin = 9;
const int echoPin = 10;

// → BLE “distance” service & characteristic (custom 16-bit UUIDs)
BLEService sensorService("180C");
BLEUnsignedIntCharacteristic distanceChar("5678", BLERead | BLENotify);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("distanceMonitor");
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(distanceChar);
  BLE.addService(sensorService);
  distanceChar.writeValue(0);   
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");
}

void loop() {
  // Wait here until a central connects
  BLEDevice central = BLE.central();
  if (!central) return;

  Serial.print("Connected to central: ");
  Serial.println(central.address());
  digitalWrite(LED_BUILTIN, HIGH);

  while (central.connected()) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);

    float distance = (duration * 0.0343) / 2.0;

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    distanceChar.writeValue(distance);

    delay(200);
  }

  // Central disconnected
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
}
