#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// verbose debug output
#define VERBOSE 0  // 0, not verbose; >0 verbose; >1 more verbose
                   // note that this will decrease speed

/* graphing data output
 * data comes out as comma separated values in the format...
 * A1 max, A2 max, B1 max, ... C2 max
 * A1 min, A2 min, B1 min, ... C2 min
 * (blank line)
 * A1 filt, A1 raw, A2 filt, A2 raw, B1 filt, ... C2 raw 
 * (previous line repeated indefinitely)
 */
#define GRAPH 0  // 0, no graph data; >0, graph data

#define DEMO 0 // 0, regular mode; >0, demo mode


// constants for sensor readings/movement
#define SMOOTH      5  // smoothing factor, 5-50
#define TOLERANCE  30  // 3% of 1024, can probably be lowered moving foward with speed control

#define ACTUATOR_STROKE_LENGTH 12  // inches
#define RELATIVE_MAX 1
#define RELATIVE_MIN 0
#define SPEED 0.5  // inches/sec
#define CALIB_SPEED_RATIO 0.25    // calibration speed relative to full speed
#define SECOND_PROBE_LENGTH 0.1   // how much the actuator should retract/extend for the second calibration probe (0.1-0.25)
#define EXTRA_SECONDS 0.7         // if the actuators aren't reaching the limits, increase this value until they do

#define MAX_ROTATION (PI/6)

// calibration settings storage
#define EEPROM_VERSION 1 // 0 to calibrate every time, 1 to load from EEPROM
#define MAX_CYCLES 10    // how many cycles before recalibrating

#define ADDR_VERSION  0                    // 1 byte, eeprom version
#define ADDR_CYCLES   (ADDR_VERSION + 1)   // 1 byte, how many cycles have passed

#define ADDR_CALIB_A1 (ADDR_CYCLES + 1)    // calibration settings, 2 16 bit integers (4 bytes) each
#define ADDR_CALIB_A2 (ADDR_CALIB_A1 + 4) 
#define ADDR_CALIB_B1 (ADDR_CALIB_A2 + 4) 
#define ADDR_CALIB_B2 (ADDR_CALIB_B1 + 4) 
#define ADDR_CALIB_C1 (ADDR_CALIB_B2 + 4) 
#define ADDR_CALIB_C2 (ADDR_CALIB_C1 + 4)

#define ADDR_CRC      (ADDR_CALIB_C2 + 4)  // cyclic redundancy check to see if EEPROM was changed, 1 long (4 bytes)


// other constants
#define NUM_ACTUATORS 6
#define NUM_CALIB_STAGES 9


// control/feedback pins
#define SHDN_BTN 7

#define IN1_A1  32
#define IN2_A1  33
#define ENA_A1   8
#define FEED_A1 A0  

#define IN1_A2  30
#define IN2_A2  31
#define ENA_A2   9
#define FEED_A2 A1

#define IN1_B1  28
#define IN2_B1  29
#define ENA_B1  10
#define FEED_B1 A2

#define IN1_B2  26
#define IN2_B2  27
#define ENA_B2  11
#define FEED_B2 A3

#define IN1_C1  24
#define IN2_C1  25
#define ENA_C1  12
#define FEED_C1 A4

#define IN1_C2  22
#define IN2_C2  23
#define ENA_C2  13
#define FEED_C2 A5


// coordinates/dimensions for inverse kinematics in inches
#define JOINT_INITIAL_HEIGHT  15.46  // z distance from joint center to center
#define JOINT_HEIGHT           2.54  // joint height from center to base/platform mate
#define MATERIAL_THICKNESS     1.50  // thickness of base and platform material
#define INITIAL_HEIGHT    JOINT_INITIAL_HEIGHT

#define BASE_Z_OFFSET  (MATERIAL_THICKNESS / 2 + JOINT_HEIGHT) // z distance from center of base to joints 
#define PLAT_Z_OFFSET -(BASE_Z_OFFSET)                         // z distance from center of platform to center of joints 

#define ACTUATOR_MIN_LENGTH    17.9
#define ACTUATOR_MAX_LENGTH    22.9

// base joint positions relative to base center
#define A1_BASE_X -21.55
#define A1_BASE_Y   -1.5

#define A2_BASE_X   9.47
#define A2_BASE_Y -19.41

#define B1_BASE_X  12.07
#define B1_BASE_Y -17.91

#define B2_BASE_X  12.07
#define B2_BASE_Y  17.91

#define C1_BASE_X   9.47
#define C1_BASE_Y  19.41

#define C2_BASE_X -21.55
#define C2_BASE_Y    1.5

// platform joint positions relative to platform center
#define A1_PLAT_X  -6.93
#define A1_PLAT_Y  -9.00

#define A2_PLAT_X  -4.33
#define A2_PLAT_Y -10.50

#define B1_PLAT_X  11.26
#define B1_PLAT_Y  -1.50

#define B2_PLAT_X  11.26
#define B2_PLAT_Y   1.50

#define C1_PLAT_X  -4.33
#define C1_PLAT_Y  10.50

#define C2_PLAT_X  -6.93
#define C2_PLAT_Y   9.00

#endif
