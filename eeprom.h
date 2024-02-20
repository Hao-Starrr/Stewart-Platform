#ifndef FLASH_H
#define FLASH_H

uint32_t EEPROM_crc(int addr);

void EEPROM_updateVersion();
void EEPROM_updateCycles();

void saveConfig(Platform &platform);
void loadConfig(Platform &platform);

#endif
