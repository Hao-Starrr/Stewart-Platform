#ifndef PLATFORM_H
#define PLATFORM_H

#include "configuration.h"
#include "Actuator.h"
#include "Vector3.h"

class Platform {
  private:
    bool isCalibrated;
    bool isReady;
    Vector3 translation, rotation, initialHeight;
    Vector3 l[NUM_ACTUATORS];

    Vector3 baseJoint[NUM_ACTUATORS] = {
      Vector3(A1_BASE_X, A1_BASE_Y, BASE_Z_OFFSET),
      Vector3(A2_BASE_X, A2_BASE_Y, BASE_Z_OFFSET),
      Vector3(B1_BASE_X, B1_BASE_Y, BASE_Z_OFFSET),
      Vector3(B2_BASE_X, B2_BASE_Y, BASE_Z_OFFSET),
      Vector3(C1_BASE_X, C1_BASE_Y, BASE_Z_OFFSET),
      Vector3(C2_BASE_X, C2_BASE_Y, BASE_Z_OFFSET),
    };

    Vector3 platformJoint[NUM_ACTUATORS] = {
      Vector3(A1_PLAT_X, A1_PLAT_Y, PLAT_Z_OFFSET),
      Vector3(A2_PLAT_X, A2_PLAT_Y, PLAT_Z_OFFSET),
      Vector3(B1_PLAT_X, B1_PLAT_Y, PLAT_Z_OFFSET),
      Vector3(B2_PLAT_X, B2_PLAT_Y, PLAT_Z_OFFSET),
      Vector3(C1_PLAT_X, C1_PLAT_Y, PLAT_Z_OFFSET),
      Vector3(C2_PLAT_X, C2_PLAT_Y, PLAT_Z_OFFSET),
    };
    
    Actuator actuators[NUM_ACTUATORS] = {
      Actuator(IN1_A1, IN2_A1, ENA_A1, FEED_A1),
      Actuator(IN1_A2, IN2_A2, ENA_A2, FEED_A2),
      Actuator(IN1_B1, IN2_B1, ENA_B1, FEED_B1),
      Actuator(IN1_B2, IN2_B2, ENA_B2, FEED_B2),
      Actuator(IN1_C1, IN2_C1, ENA_C1, FEED_C1),
      Actuator(IN1_C2, IN2_C2, ENA_C2, FEED_C2),
    };
    
    float convertLengthToRelative(Vector3 &v);
    void calculateLengths();
    float clip(float f);
    
    bool actuatorIsValid(int actuator);

  public:
    Platform();

    void setup();
    void loop();

    void calibrate();
    void calibrate(uint16_t (&settings)[NUM_ACTUATORS][2]);
    void retract();
    void extend();
    void setHeight(float height);
    
    bool setPlatformLengths(float (&lengths)[NUM_ACTUATORS]);
    bool setPlatformPosition(float (&positions)[6]);
    bool setPlatformPosition(float x, float y, float z, float roll, float pitch, float yaw);
    bool setPlatformPosition(Vector3 &t, Vector3 &r);
    
    int getActuatorRawPosition(int actuator);
    int getActuatorPosition(int actuator);
    int getActuatorTarget(int actuator);
    int getActuatorMaxPosition(int actuator);
    int getActuatorMinPosition(int actuator);

    bool isActuatorReady(int actuator);
    bool isPlatformReady();
};

#endif
