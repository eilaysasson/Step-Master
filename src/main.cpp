#include <Arduino.h>
#include <ArduinoBLE.h>


BLEService stepService("180A");

BLEIntCharacteristic stepCharacteristic(
  "2A57",
  BLERead | BLENotify
);


uint32_t stepCount = 0;

unsigned long lastStep = 0;


#define LED_PIN LED_BUILTIN


void setup()
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);


    Serial.begin(115200);

    delay(1000);

    Serial.println("StepMaster BLE Simulator");


    if (!BLE.begin())
    {
        Serial.println("BLE initialization failed!");
        while (1);
    }


    BLE.setLocalName("StepMaster_XIAO");

    BLE.setAdvertisedService(stepService);


    stepService.addCharacteristic(stepCharacteristic);

    BLE.addService(stepService);


    stepCharacteristic.writeValue(stepCount);


    BLE.advertise();


    Serial.println("BLE advertising started");
}



void loop()
{
    BLE.poll();


    if (millis() - lastStep >= 10000)
    {
        lastStep = millis();

        stepCount++;


        Serial.print("STEP: ");
        Serial.println(stepCount);


        // LED ON 200ms
        digitalWrite(LED_PIN, LOW);
        delay(200);
        digitalWrite(LED_PIN, HIGH);



        // BLE update
        stepCharacteristic.writeValue(stepCount);


        Serial.println("BLE step notification sent");
    }
}