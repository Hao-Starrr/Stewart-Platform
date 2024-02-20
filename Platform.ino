#include "Platform.h"
#include "eeprom.h"

Platform::Platform() {
  isCalibrated = false;
  isReady = false;
  translation.Set(0, 0, 0);
  rotation.Set(0, 0, 0);
  for (int i = 0; i < NUM_ACTUATORS; i++) {
    l[i].Set(0, 0, 0);
  }
}

void Platform::setup() {
  for (int i = 0; i < NUM_ACTUATORS; i++) {
    actuators[i].setup();
  }
  initialHeight.Set(0, 0, INITIAL_HEIGHT);
  loadConfig(*this);
}

void Platform::loop() {
  for (int i = 0; i < NUM_ACTUATORS; i++) {
    actuators[i].loop();
  }
}

void Platform::calibrate() {
  for (int stage = 0; stage <= NUM_CALIB_STAGES; stage++) {
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      actuators[i].calibrate();
    }
  
    switch (stage) {
      case 0: 
      case 5: delay(ACTUATOR_STROKE_LENGTH / SPEED * 1000 + EXTRA_SECONDS * 1000); // wait for actuators to fully extend/retract
              break;
  
      case 2:
      case 7: delay(SECOND_PROBE_LENGTH * ACTUATOR_STROKE_LENGTH / SPEED * 1000); // wait for actuators to extend/retract to prepare second check
              break;
  
      case 3:
      case 8: delay(SECOND_PROBE_LENGTH * ACTUATOR_STROKE_LENGTH / (SPEED * CALIB_SPEED_RATIO) * 1000 + EXTRA_SECONDS * 1000); // wait for actuators to extend/retract to make second check
              break;
              
      default: break;
    }
  }
  isCalibrated = true;
  isReady = true;
}

void Platform::calibrate(uint16_t (&settings)[NUM_ACTUATORS][2]) {
  for (int i = 0; i < NUM_ACTUATORS; i++) {
    actuators[i].calibrate(settings[i]);
  }
  isCalibrated = true;
  isReady = true;
}

void Platform::retract() {
  setHeight(0);
}

void Platform::extend() {
  setHeight(1);
}

void Platform::setHeight(float height) {
  float arr[NUM_ACTUATORS];
  for (int i = 0; i < NUM_ACTUATORS; i++) {
    arr[i] = height;
  }
  setPlatformLengths(arr);
}

bool Platform::setPlatformPosition(float (&positions)[6]) {
  float x, y, z, roll, pitch, yaw;
  x = positions[0];
  y = positions[1];
  z = positions[2];
  roll = positions[3];
  pitch = positions[4];
  yaw = positions[5];
  return setPlatformPosition(x, y, z, roll, pitch, yaw);
}

bool Platform::setPlatformPosition(float x, float y, float z, float roll, float pitch, float yaw) {
  Vector3 t(x, y, z);
  Vector3 r(roll, pitch, yaw);
  return setPlatformPosition(t, r);
}

// use plateform position and rotation to set actuator coordinates, and calculate actuator lengths
bool Platform::setPlatformPosition(Vector3 &t, Vector3 &r) {
  #if VERBOSE > 0
    Serial.print("Translation: (");
    Serial.print(t.x); Serial.print(", ");
    Serial.print(t.y); Serial.print(", ");
    Serial.print(t.z); Serial.println(")");
    
    Serial.print("Rotation: (");
    Serial.print(r.x); Serial.print(", ");
    Serial.print(r.y); Serial.print(", ");
    Serial.print(r.z); Serial.println(")");
  #endif
  if (isPlatformReady()) {
    translation = t;
    r.x = clip(r.x) * MAX_ROTATION;
    r.y = clip(r.y) * MAX_ROTATION;
    r.z = clip(r.z) * MAX_ROTATION;
    rotation = r;
    calculateLengths(); // calculate the coordinates of each actuator
    float lengths[NUM_ACTUATORS];

    #if VERBOSE > 0
      Serial.println("Absolute/relative lengths: ");
    #endif
    
    // convert to relavant length
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      lengths[i] = convertLengthToRelative(l[i]);
      #if VERBOSE > 0
        Serial.print(l[i].Length()); 
        Serial.print(" / ");
        Serial.println(lengths[i]);
      #endif
    }
    
    // set target lengths to every actuator
    return setPlatformLengths(lengths); 
  } else {
    #if VERBOSE > 0
      Serial.println("Platform is not ready, position not accepted"); Serial.println();
    #endif
    return false;
  }
}

bool Platform::setPlatformLengths(float (&lengths)[6]) {
  if (isPlatformReady()) {
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      if (lengths[i] < RELATIVE_MIN || lengths[i] > RELATIVE_MAX) {
        #if VERBOSE > 0
          Serial.print("Relative length with index ");
          Serial.print(i);
          Serial.print(" is out of range at ");
          Serial.println(lengths[i]); Serial.println();
          Serial.print("Range is from "); Serial.print(RELATIVE_MIN);
          Serial.print(" to "); Serial.println(RELATIVE_MAX);
        #endif
        return false;
      }
    }
    
    #if VERBOSE > 0
      Serial.print("Lengths: ");
    #endif
    
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      #if VERBOSE > 0
        Serial.print(lengths[i]); Serial.print(" ");
      #endif
      
      // set into actuators
      actuators[i].setLength(lengths[i]);
    } 
    
    #if VERBOSE > 0
      Serial.println(); Serial.println();
    #endif

    return true;
  } else {
    return false;
  }
}

// position ik 
void Platform::calculateLengths() {
  Vector3 q[NUM_ACTUATORS];
  for (int i = 0; i < NUM_ACTUATORS; i++) {
    // rotation
    l[i].x = cos(rotation.z)*cos(rotation.y)*platformJoint[i].x +
      (-sin(rotation.z)*cos(rotation.x)+cos(rotation.z)*sin(rotation.y)*sin(rotation.x))*platformJoint[i].y +
      (sin(rotation.z)*sin(rotation.x)+cos(rotation.z)*sin(rotation.y)*cos(rotation.x))*platformJoint[i].z;

    l[i].y = sin(rotation.z)*cos(rotation.y)*platformJoint[i].x +
      (cos(rotation.z)*cos(rotation.x)+sin(rotation.z)*sin(rotation.y)*sin(rotation.x))*platformJoint[i].y +
      (-cos(rotation.z)*sin(rotation.x)+sin(rotation.z)*sin(rotation.y)*cos(rotation.x))*platformJoint[i].z;

    l[i].z = -sin(rotation.y)*platformJoint[i].x +
      cos(rotation.y)*sin(rotation.x)*platformJoint[i].y +
      cos(rotation.y)*cos(rotation.x)*platformJoint[i].z;

    #if VERBOSE > 1
      Serial.print("l["); Serial.print(i); Serial.print("] with rotation applied: ");
      l[i].printVect(); Serial.println();
    #endif

    // translation
    l[i] += translation;
    #if VERBOSE > 1
      Serial.print("l["); Serial.print(i); Serial.print("] with translation applied: ");
      l[i].printVect(); Serial.println();
    #endif
    
    l[i] += initialHeight;
    #if VERBOSE > 1
      Serial.print("l["); Serial.print(i); Serial.print("] with initial height applied: ");
      l[i].printVect(); Serial.println();
    #endif
    
    l[i] -= baseJoint[i];
    #if VERBOSE > 1
      Serial.print("l["); Serial.print(i); Serial.print("] with base joint applied: ");
      l[i].printVect(); Serial.println();
    #endif
  }
}

float Platform::convertLengthToRelative(Vector3 &v) {
  return ((float)v.Length() - ACTUATOR_MIN_LENGTH) / ACTUATOR_STROKE_LENGTH;
}

float Platform::clip(float f) {
  return max(-1, min(1, f));
}

int Platform::getActuatorRawPosition(int actuator) {
  if (actuatorIsValid(actuator)) {
    return actuators[actuator].getRawPosition();
  } else {
    return 0;
  }
}

int Platform::getActuatorPosition(int actuator) {
  if (actuatorIsValid(actuator)) {
    return actuators[actuator].getPosition();
  } else {
    return 0;
  }
}

int Platform::getActuatorTarget(int actuator) {
  if (actuatorIsValid(actuator)) {
    return actuators[actuator].getTargetPosition();
  } else {
    return 0;
  }
}

int Platform::getActuatorMaxPosition(int actuator) {
  if (actuatorIsValid(actuator)) {
    return actuators[actuator].getMaxPosition();
  } else {
    return 0;
  }
}

int Platform::getActuatorMinPosition(int actuator) {
  if (actuatorIsValid(actuator)) {
    return actuators[actuator].getMinPosition();
  } else {
    return 0;
  }
}

bool Platform::isActuatorReady(int actuator) {
  if (actuatorIsValid(actuator)) {
    return actuators[actuator].isActuatorReady();
  } else {
    return 0;
  }
}

bool Platform::isPlatformReady() {
  isReady = true;
  for (int i = 0; i < NUM_ACTUATORS; i++) {
    isReady = isReady && actuators[i].isActuatorReady();
  }
  return isReady;
}

bool Platform::actuatorIsValid(int actuator) {
  return actuator >= 0 && actuator < NUM_ACTUATORS;
}
