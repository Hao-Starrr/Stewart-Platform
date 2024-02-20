#include <EEPROM.h>
#include "configuration.h"
#include "Platform.h"

uint32_t EEPROM_crc(int addr)
{
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;

  for (int index = 0 ; index < addr  ; ++index) {
    crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}

void EEPROM_updateVersion() {
  EEPROM.update(ADDR_VERSION, EEPROM_VERSION);
}

void EEPROM_updateCycles() {
  uint8_t cycles = 0;
  
  if (EEPROM_VERSION == 1) {
    cycles = EEPROM.read(ADDR_CYCLES);
    cycles++;
  
    if (cycles >= MAX_CYCLES) {
      cycles = 0;
    }
  }

  EEPROM.update(ADDR_CYCLES, cycles);
}

void saveConfig(Platform &platform) {
   uint16_t a1[] = {
     (uint16_t)platform.getActuatorMinPosition(0),
     (uint16_t)platform.getActuatorMaxPosition(0)
   };
     
   uint16_t a2[] = {
     (uint16_t)platform.getActuatorMinPosition(1),
     (uint16_t)platform.getActuatorMaxPosition(1)
   };
   uint16_t b1[] = {
     (uint16_t)platform.getActuatorMinPosition(2),
     (uint16_t)platform.getActuatorMaxPosition(2)
   };
   uint16_t b2[] = {
     (uint16_t)platform.getActuatorMinPosition(3),
     (uint16_t)platform.getActuatorMaxPosition(3)
   };
   uint16_t c1[] = {
     (uint16_t)platform.getActuatorMinPosition(4),
     (uint16_t)platform.getActuatorMaxPosition(4)
   };
   uint16_t c2[] = {
     (uint16_t)platform.getActuatorMinPosition(5),
     (uint16_t)platform.getActuatorMaxPosition(5)
   };

   EEPROM.put(ADDR_CALIB_A1, a1);
   EEPROM.put(ADDR_CALIB_A2, a2);
   EEPROM.put(ADDR_CALIB_B1, b1);
   EEPROM.put(ADDR_CALIB_B2, b2);
   EEPROM.put(ADDR_CALIB_C1, c1);
   EEPROM.put(ADDR_CALIB_C2, c2);
}

void loadConfig(Platform &platform) {
  uint32_t crcOld;
  EEPROM.get(ADDR_CRC, crcOld);
  uint32_t crcNew = EEPROM_crc(ADDR_CRC);
  uint8_t cycles = 0;

  if (crcOld != crcNew) {
    EEPROM_updateVersion();
    EEPROM.put(ADDR_CYCLES, cycles);
  }
  
  if (EEPROM_VERSION == 0) {
    platform.calibrate();
  } else if (EEPROM_VERSION == 1) {
    cycles = EEPROM.read(ADDR_CYCLES);
    if (cycles == 0) {
      platform.calibrate();
      saveConfig(platform);
    } else {
      uint16_t a1[2], a2[2], b1[2], b2[2], c1[2], c2[2];
      EEPROM.get(ADDR_CALIB_A1, a1);
      EEPROM.get(ADDR_CALIB_A2, a2);
      EEPROM.get(ADDR_CALIB_B1, b1);
      EEPROM.get(ADDR_CALIB_B2, b2);
      EEPROM.get(ADDR_CALIB_C1, c1);
      EEPROM.get(ADDR_CALIB_C2, c2);

      uint16_t settings[NUM_ACTUATORS][2] = {
        {a1[0], a1[1]},
        {a2[0], a2[1]},
        {b1[0], b1[1]},
        {b2[0], b2[1]},
        {c1[0], c1[1]},
        {c2[0], c2[1]},
      };
      platform.calibrate(settings);
    }
    
    EEPROM_updateCycles();
    crcNew = EEPROM_crc(ADDR_CRC);
    EEPROM.put(ADDR_CRC, crcNew);
  }
}
