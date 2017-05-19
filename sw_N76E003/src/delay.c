#include "../inc/delay.h"

bit BIT_TMP;

void DelayTick(u8 delay)
{
	while(delay--);
}

void Delayus(void){
	DelayTick(250);
	DelayTick(250);
	DelayTick(250);
}	
void Delayms(u16 ms){
	while(ms--)
	{
		Delayus();	 
	}
}

