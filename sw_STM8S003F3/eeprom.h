#include "typedef.h"



#ifndef __EEPROM_H__
#define __EEPROM_H__


#define EEPROM_ADDR_BASE      0x4000
#define SET_FLAG_STR_ADDR (EEPROM_ADDR_BASE + 0)
#define TEMP_SET_ADDR     (EEPROM_ADDR_BASE + 16)
#define TIME_SET_ADDR     (EEPROM_ADDR_BASE + 18)

#define HEAT1_TIME_ADDR   (EEPROM_ADDR_BASE + 20)
#define HEAT2_TIME_ADDR   (EEPROM_ADDR_BASE + 24)
#define HEAT3_TIME_ADDR   (EEPROM_ADDR_BASE + 28)

#define EepromReadByte(addr)  (*((u8*)addr))
u16 EepromReadWord(u8 * addr);
u32 EepromReadDword(u8 * addr); 

void EepromProgramByte(u8 * addr, u8 dat);
void EepromProgramWord(u8 * addr, u16 dat);
void EepromProgramDword(u8 * addr, u32 dat);


#endif
//==========================================================
//end files
