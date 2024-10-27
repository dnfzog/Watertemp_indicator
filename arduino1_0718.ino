#include <Arduino.h>
#include <SensirionI2CSdp.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
 
SensirionI2CSdp sdp;
File myFile;
const int chipSelect = 10;  // チップセレクトピンの指定
 
void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }
 
    Wire.begin();
 
    uint16_t error;
    char errorMessage[64];
 
    sdp.begin(Wire, SDP8XX_I2C_ADDRESS_0);
 
    uint32_t productNumber;
    uint8_t serialNumber[8];
    uint8_t serialNumberSize = 8;
 
    sdp.stopContinuousMeasurement();
 
    error = sdp.readProductIdentifier(productNumber, serialNumber, serialNumberSize);
    if (error) {
        Serial.print(F("Error trying to execute readProductIdentifier(): "));
        errorToString(error, errorMessage, 64);
        Serial.println(errorMessage);
    } else {
        Serial.print(F("ProductNumber:"));
        Serial.print(productNumber);
        Serial.print(F("\tSerialNumber:0x"));
        for (size_t i = 0; i < serialNumberSize; i++) {
            Serial.print(serialNumber[i], HEX);
        }
        Serial.println();
    }
 
    error = sdp.startContinuousMeasurementWithDiffPressureTCompAndAveraging();
 
    if (error) {
        Serial.print(F("Error trying to execute startContinuousMeasurementWithDiffPressureTCompAndAveraging(): "));
        errorToString(error, errorMessage, 64);
        Serial.println(errorMessage);
    }
 
    Serial.println(F("Initializing SD card..."));
 
    // SPI速度を低くしてSDカードを初期化
    if (!SD.begin(chipSelect, SPI_HALF_SPEED)) {
        Serial.println(F("Initialization failed or does not exist"));
        while (1);
    }
    Serial.println(F("Initialization done."));
    Serial.println();
 
    delay(1000);
    Serial.println(F("Count Start"));
 
    myFile = SD.open("DATA_LOG.csv", FILE_WRITE);
    if (myFile) {
        myFile.println(F("Count Start"));
        myFile.close();
    } else {
        Serial.println(F("Error opening file"));
    }
}
 
void loop() {
    uint16_t error;
    char errorMessage[64];
 
    delay(1000);
 
    float differentialPressure;
    float temperature;
 
    error = sdp.readMeasurement(differentialPressure, temperature);
 
    if (error) {
        Serial.print(F("Error trying to execute readMeasurement(): "));
        errorToString(error, errorMessage, 64);
        Serial.println(errorMessage);
    } else {
        Serial.print(F("DifferentialPressure[Pa]:"));
        Serial.print(differentialPressure);
        Serial.print(F("\tTemperature[°C]:"));
        Serial.print(temperature);
        Serial.println();
    }
 
    myFile = SD.open("DATA_LOG.csv", FILE_WRITE);
    if (myFile) {
        myFile.print(F("DifferentialPressure[Pa]:"));
        myFile.print(differentialPressure);
        myFile.print(F("\tTemperature[°C]:"));
        myFile.print(temperature);
        myFile.println();
        myFile.close();
    } else {
        Serial.println(F("Error opening file"));
    }
}