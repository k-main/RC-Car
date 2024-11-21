#include <Wire.h>
#include <TinyGPS.h>
#include <MPU6050.h>

// GPS Setup
TinyGPS gps;
HardwareSerial gpsSerial(2);
// MPU6050 Setup
MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  while (!Serial); // wait for serial monitor
  
  // start GPS
  gpsSerial.begin(4800, SERIAL_8N1, 16, 17); // TX=16, RX=17
  Serial.println("GPS and MPU6050 Combined Code");

  // start MPU6050
  Wire.begin(21, 22); // SDA=21, SCL=22
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful");

    // automatic calibration (leave the unit standing still until 5 seconds)
    Serial.println("Calibrating gyroscope...");
    mpu.CalibrateGyro(6);
    Serial.println("Calibration complete.");
    mpu.PrintActiveOffsets();

    // // for manual calibration
    // mpu.setXGyroOffset(offsetValueX);
    // mpu.setYGyroOffset(offsetValueY);
    // mpu.setZGyroOffset(offsetValueZ);
  } 
  else {
    Serial.println("MPU6050 connection failed");
    while (1); // stop if connection fail
  }
}

void loop() {
  // READ GPS DATA (extremely unreliable while indoors and with small movements)
  while (gpsSerial.available()) {
    char c = gpsSerial.read();
    if (gps.encode(c)) {
      // only print updated GPS data
      float latitude, longitude;
      unsigned long age;
      gps.f_get_position(&latitude, &longitude, &age);
      Serial.print("GPS: LAT=");
      Serial.print(latitude == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : latitude, 6);
      Serial.print(" LON=");
      Serial.print(longitude == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : longitude, 6);
      Serial.println();
    }
  }

  // READ MPU6050 DATA (average of 10 readings)
  const int numReadings = 10;
  int32_t sumAx = 0, sumAy = 0, sumAz = 0;
  int32_t sumGx = 0, sumGy = 0, sumGz = 0;

  for (int i = 0; i < numReadings; i++) {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sumAx += ax;
    sumAy += ay;
    sumAz += az;
    sumGx += gx;
    sumGy += gy;
    sumGz += gz;
    delay(5); // prevent overloading
  }

  int16_t avgAx = sumAx / numReadings;
  int16_t avgAy = sumAy / numReadings;
  int16_t avgAz = sumAz / numReadings;
  int16_t avgGx = sumGx / numReadings;
  int16_t avgGy = sumGy / numReadings;
  int16_t avgGz = sumGz / numReadings;

  Serial.print("MPU6050: ");
  Serial.print("Acc[X]="); Serial.print(avgAx);
  Serial.print(" Acc[Y]="); Serial.print(avgAy);
  Serial.print(" Acc[Z]="); Serial.print(avgAz);
  Serial.print(" Gyro[X]="); Serial.print(avgGx);
  Serial.print(" Gyro[Y]="); Serial.print(avgGy);
  Serial.print(" Gyro[Z]="); Serial.println(avgGz);

  // loop delay
  delay(100);
}
