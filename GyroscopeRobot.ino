/* Get all possible data from MPU6050
 * Accelerometer values are given as multiple of the gravity [1g = 9.81 m/s²]
 * Gyro values are given in deg/s
 * Angles are given in degrees
 * Note that X and Y are tilt angles and not pitch/roll.
 *
 * License: MIT
 */

#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
int i1 = 3;
int i2 = 5;
int i3 = 10;
int i4 = 11;
long timer = 0;

float kP = 25.0;
float kI = 0;
float kD = 0;
float wantedAngle = 0 ;
float lowestGyro = 100;

float integral = 0.0;
float error = 0.0;
float startTarget = 0.0;
float derivative = 0.0;
float prevError = 0.0;
int result = 0;
float start = 0.0;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(i1, OUTPUT);
  pinMode(i2, OUTPUT);
  pinMode(i3, OUTPUT);
  pinMode(i4, OUTPUT);

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(true,true); // gyro and accelero
  Serial.println("Done!\n");
  
}

void loop() {
  mpu.update();
  float gyroFeedback = mpu.getGyroX() * 0; // Adjust scale if needed
  wantedAngle += gyroFeedback;
  wantedAngle = constrain(wantedAngle, -5.0, 5.0);
  error = wantedAngle - mpu.getAngleX();
  integral = integral + (error * 0.25);
  integral = constrain(integral, -50, 50);  

  derivative = error - prevError;
  prevError = error;
  float pidOutput = (error * kP) + (integral * kI) + (derivative * kD);
 
  result = constrain(abs(pidOutput), 50, 255);  // Minimum speed is 50
  int badResult = constrain(result *1.1, 50, 255);
  float smoothedAngleX = 0.9 * smoothedAngleX + 0.1 * mpu.getAngleX();
  if (abs(smoothedAngleX) < 0.1) {
    smoothedAngleX = 0; // Treat small changes as 0
  }
  if (smoothedAngleX >= 0) {
    analogWrite(i1, 0);
    analogWrite(i2, badResult);
    analogWrite(i3, 0);
    analogWrite(i4, result);
  } else {
    analogWrite(i1, badResult);
    analogWrite(i2, 0);
    analogWrite(i3, result);  
    analogWrite(i4, 0);
  }
  
  if(millis() - timer > 300){ // print data every second
  
  

    
    Serial.println(F("ANGLE     X: "));Serial.print(smoothedAngleX);

    Serial.println(F("=====================================================\n"));
    timer = millis();
  }

}
