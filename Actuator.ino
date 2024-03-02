#include "Actuator.h"

// 直线电机伸长
void Actuator::Actuator::extend() {
  extend(255);
}

void Actuator::Actuator::extend(uint8_t pwm) {
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
  analogWrite(enaPin, pwm);
}

// 直线电机缩回
void Actuator::retract() {
  retract(255);
}

void Actuator::retract(uint8_t pwm) {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
  analogWrite(enaPin, pwm);
}

// 直线电机关闭
void Actuator::off() { // leave motor floating
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, HIGH);
  digitalWrite(enaPin, LOW);
}

// 直线电机刹车
void Actuator::brake() {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
  digitalWrite(enaPin, LOW);
}

// 根据两个waypoint之间的距离为每个电机设置速度，原因是两个waypoint之间，每个电机要走的行程不一样，但是期望同时到达
int Actuator::calculateSpeeds(){
  relativeDifference = (targetPosition - filtPosition) / ACTUATOR_STROKE_LENGTH;
  speedPwm = abs(100 * relativeDifference);
  return speedPwm;
}

// 平滑feedback值，主要功能是更新filtPosition的值
void Actuator::readPosition() {
  total -= readings[index];                // subtract the last value
  rawPosition = analogRead(feedbackPin);   // read the sensor
  readings[index] = rawPosition;           // record the reading
  total += readings[index];                // add the value
  filtPosition = total / SMOOTH;           // average reading and update the filtered position

  index++;
  if (index >= SMOOTH) {  // reset index if at end of array
    index = 0;
  }
}

   
Actuator::Actuator(byte attachToIn1Pin, byte attachToIn2Pin, byte attachToEnaPin, byte attachToFeedbackPin) :
  in1Pin(attachToIn1Pin),
  in2Pin(attachToIn2Pin),
  enaPin(attachToEnaPin),
  feedbackPin(attachToFeedbackPin) {
     
  isCalibrated = false;
  calibrationStage = 0; 
  maxPosition = 0;
  minPosition = 0;
  
  isReady = false;
  memset(readings, 0, sizeof(readings));
  index = 0;
  total = 0;
  targetPosition = 512;
  readPosition();
}

// 初始化，设置pinmode，置于关闭状态
void Actuator::setup() {
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enaPin, OUTPUT);
  pinMode(feedbackPin, INPUT);
  off();
}


// 给定一个targetposition的情况下，运行loop，actuator就会运动到targetposition
// 这个函数确切应该叫loop once
void Actuator::loop() {
  readPosition();
  float Kp = 2.0;
  if (isCalibrated && !isReady) {
    if (filtPosition <= targetPosition + TOLERANCE && filtPosition >= targetPosition - TOLERANCE) { 
      // brake();
      isReady = true;
    } 
    else if (filtPosition > targetPosition + TOLERANCE) { 
      float speed = Kp* (filtPosition - targetPosition) + 10;
      uint8_t constainedSpeed = min(255,speed);
      // Serial.println(constainedSpeed);
      // extend(constainedSpeed);
      extend();

    } 
    else if (filtPosition < targetPosition - TOLERANCE) { 
      float speed = Kp* (targetPosition - filtPosition) + 10;
      uint8_t constainedSpeed = min(255,speed);
      // Serial.println(constainedSpeed);

      // retract(constainedSpeed);
      retract();

    }
  }
}

/* calibration stages
   0 - extend for initial max 
   1 - set initial max 
   2 - retract for final max
   3 - extend for final max
   4 - set final max
   5 - retract for initial min
   6 - set initial min
   7 - extend for final min
   8 - retract for final min
   9+ - set final min, done   */

// 自动标定程序
void Actuator::calibrate() {
  switch (calibrationStage) {
    case 0: 
    case 7: extend();
            break;

    case 3: extend(255 * CALIB_SPEED_RATIO);
            break;

    case 2:
    case 5: retract();
            break;
    
    case 8: retract(255 * CALIB_SPEED_RATIO);
            break;
            
    case 1:
    case 4:
    case 6:
    case 9: for(int i = 0; i < SMOOTH; i++)
              readPosition();
            brake();
            break;
    
    default:break;
  }

  switch (calibrationStage) {
    case 1: minPosition = filtPosition;
            break;

    case 4: minPosition = max(minPosition, filtPosition);
            break;

    case 6: maxPosition = filtPosition;
            break;

    case 9: maxPosition = min(maxPosition, filtPosition);
            break;

    default: break;
  }
  calibrationStage++;
  if(calibrationStage > NUM_CALIB_STAGES) {
    calibrationStage = 0;
    isCalibrated = true;
    isReady = true;
  }
}

// ?限制最大最小位置？
void Actuator::calibrate(uint16_t (&settings)[2]) {
  maxPosition = settings[0];
  minPosition = settings[1];
  isCalibrated = true;
  isReady = true;
}

// 通过minposition，maxposition和relativeLength来确定targetposition
void Actuator::setLength(float relativeLength) {
  if (isCalibrated && isReady) {
    isReady = false;
    if (relativeLength < 0) {
      relativeLength = 0;
    } else if (relativeLength > 1) {
      relativeLength = 1;
    }
    targetPosition = minPosition + (maxPosition - minPosition) * relativeLength;
  }
}

bool Actuator::isActuatorReady() {
  return isReady;
}

int Actuator::getRawPosition() {
  return rawPosition;
}

int Actuator::getPosition() {
  return filtPosition;
}

int Actuator::getTargetPosition() {
  return targetPosition;
}

int Actuator::getMaxPosition() {
  return maxPosition;
}

int Actuator::getMinPosition() {
  return minPosition;
}
