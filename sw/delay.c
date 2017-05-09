#include "delay.h"

void DelayTick(u8 delay)
{
	while(delay--);
}

void Delayus(u16 us){
	DelayTick(250);
	DelayTick(250);
	DelayTick(250);
}	
void Delayms(u16 ms){
	while(ms--)
	{
		Delayus(1000);	 
	}
}

