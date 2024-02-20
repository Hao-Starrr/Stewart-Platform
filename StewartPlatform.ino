/**
 * stewart-platform
 * 
 * Authors: Leroy Sibanda, Michael Rosa
 * Date: August 2018
 * 
 * Program to test and calibrate an Stewart platform based on an Arduino Mega and motor drivers
 */

#include "configuration.h"
#include "Actuator.h"
#include "Platform.h"
#include "eeprom.h"

int count2 = 0;

#if DEMO > 0
  float positions[][6] = {
    {0,0,0,0,0,0},
    {0,0,15,0,0,0},
    
    {0,-5,5,0,0,0},
    {0,5 ,5,0,0,0},
    
    {-5,0,5,0,0,0},
    {5,0,5,0,0,0},
    
    {0,0,10,-1,0,0},
    {0,0,10,1,0,0},
    
    {0,0,5,0,-1,0},
    {0,0,5,0,1,0},
    
    {0,0,10,0,0,-1},
    {0,0,10,0,0,1},
  };
#else
  #define POINTS 20
#endif


// ------ main program ------ //
Platform platform;
  
void setup() {
  Serial.begin(9600);

  // set the analog ref (5V regulator)
  analogReference(EXTERNAL);

  pinMode(SHDN_BTN, INPUT_PULLUP);

  platform.setup();

  #if VERBOSE > 0
    Serial.println("Actuator info");
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      Serial.print("Actuator "); Serial.print(i); 
      Serial.print(" position: ");
      Serial.println(platform.getActuatorPosition(i));
      Serial.print("min: "); 
      Serial.println(platform.getActuatorMinPosition(i));
      Serial.print("max: "); 
      Serial.println(platform.getActuatorMaxPosition(i));
      Serial.print("target: ");
      Serial.println(platform.getActuatorTarget(i));
      Serial.print("ready: ");
      Serial.println(platform.isActuatorReady(i));
      Serial.println();
    } 
  #endif

  #if GRAPH > 0
    // print the min and max positions of each actuator
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      Serial.print(platform.getActuatorMaxPosition(i));
      Serial.print(',');
    }
    Serial.println();
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      Serial.print(platform.getActuatorMinPosition(i));
      Serial.print(',');
    }
    Serial.println();
    Serial.println();
  #endif
}

void loop() {
  if(digitalRead(SHDN_BTN) == LOW) {
    platform.retract();
  } else {
    #if DEMO > 0
      if (platform.isPlatformReady()) {
        platform.setPlatformPosition(positions[count2]);
        count2++;
        if (count2 >= sizeof(positions) / NUM_ACTUATORS / sizeof(int)) {
          count2 = 0;
        }
      }
    #else
      if (platform.isPlatformReady()) {
        float ratio = (float)count2 / POINTS;
        float p[] = {5*cos(TWO_PI * ratio), 5*sin(TWO_PI * ratio), 5*sin(TWO_PI * ratio * 3)+6, 
                      0.5*sin(TWO_PI * ratio),0.5*cos(TWO_PI * ratio),0.33*sin(TWO_PI * ratio)};
        platform.setPlatformPosition(p);
        
        count2++;
        if (count2 > POINTS) {
          count2 = 0;
        }
      }
    #endif
  }
  
  platform.loop();

  #if GRAPH > 0
    // print the current raw and filtered positions of each actuator
    for (int i = 0; i < NUM_ACTUATORS; i++) {
      Serial.print(platform.getActuatorPosition(i));
      Serial.print(',');
      Serial.print(platform.getActuatorRawPosition(i));
      Serial.print(',');
    } 
    Serial.println();
  #endif
}
