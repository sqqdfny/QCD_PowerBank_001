#include "eeprom.h"


//==========================================================
void UnlockEeprom(void)
{
	do
	{
		FLASH_DUKR = 0xAE; 
		FLASH_DUKR = 0x56; 
	}
	while(0 == (FLASH_IAPSR & BIT3));
}

void LockEeprom(void)
{
		FLASH_IAPSR &= ~BIT3; 
}

void EepromProgramByte(u8 * addr, u8 dat)
{
	UnlockEeprom();
	FLASH_CR2 = 0x00;
	FLASH_NCR2 = 0xff;
	*(addr) = dat;
	while(0 == (FLASH_IAPSR & BIT2));
	LockEeprom();
}

void EepromProgramWord(u8 * addr, u16 dat)
{
	UnlockEeprom();
	FLASH_CR2 = 0x00;
	FLASH_NCR2 = 0xff;
	*addr = (u8)(dat >> 8);
	while(0 == (FLASH_IAPSR & BIT2));
	*(addr + 1) = (u8)(dat);
	while(0 == (FLASH_IAPSR & BIT2));
	LockEeprom();
}

void EepromProgramDword(u8 * addr, u32 dat)
{
	UnlockEeprom();
	FLASH_CR2 = BIT6;
	FLASH_NCR2 = ~BIT6;

	*(addr    ) = *(((u8*)(&dat)) + 0);
	*(addr + 1) = *(((u8*)(&dat)) + 1);
	*(addr + 2) = *(((u8*)(&dat)) + 2);
	*(addr + 3) = *(((u8*)(&dat)) + 3);
	while(0 == (FLASH_IAPSR & BIT2));
	LockEeprom();
}

u16 EepromReadWord(u8 * addr)  
{
	u16 tmp  = EepromReadByte(addr++);
	tmp = (tmp << 8) + EepromReadByte(addr);
	return (tmp);
}
u32 EepromReadDword(u8 * addr) 
{
	u32 tmp  = EepromReadByte(addr++);
	tmp = (tmp << 8) + EepromReadByte(addr ++);
	tmp = (tmp << 8) + EepromReadByte(addr ++);
	tmp = (tmp << 8) + EepromReadByte(addr);
	return (tmp);
}
//==========================================================
//end files
