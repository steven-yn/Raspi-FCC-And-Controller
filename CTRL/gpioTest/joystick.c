#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <math.h>

int spi_channel=0;
int spi_speed=1000000;
int adc_channel=0;
	
unsigned char ch0buffer[3], ch1buffer[3], ch6buffer[3], ch7buffer[3];
int adc_value;
int volt;
int preus;
int cnt, flag;

int main()
{
	wiringPiSetup();
	if(wiringPiSPISetup( spi_channel,spi_speed )==-1)
	return-1;
	
	while(1)
	{
		unsigned long curus = millis(); // each 1ms ++curr~
		if(curus - preus > 100) // more than 1000ms
		{
			preus = curus; // each 1ms pre~ += 1000
			if ( cnt++ >= 4 )
			{
				cnt = 0;
			}
			flag++;
		}
		
		switch (cnt)
		{
			case 0:
				
				adc_channel=0;
				
				ch0buffer[0]=1;
				ch0buffer[1]=(8+adc_channel)<<4;
				ch0buffer[2]=0;
				
				wiringPiSPIDataRW(spi_channel, ch0buffer, 3);
				
				adc_value = ((ch0buffer[1])<<8)+ch0buffer[2];
				volt = ((ch0buffer[1]&3)<<8)+ch0buffer[2];
				if ( flag >= 1 )
				{
					printf("ch0 value: %4d voltage: %d\n", adc_value, 5*volt / 1000);
					flag =0;
				}
				break;
				
			case 1:
				adc_channel=1;
				
				ch1buffer[0]=1;
				ch1buffer[1]=(8+adc_channel)<<4;
				ch1buffer[2]=0;
				
				wiringPiSPIDataRW(spi_channel, ch1buffer, 3);
				
				adc_value = ((ch1buffer[1])<<8)+ch1buffer[2];
				volt = ((ch1buffer[1]&3)<<8)+ch1buffer[2];
				if ( flag >= 1 )
				{
					printf("ch1 value: %4d voltage: %d\n", adc_value, 5*volt / 1000);
					flag =0;
				}
				break;
				
			case 2:
				adc_channel=6;
				
				ch6buffer[0]=1;
				ch6buffer[1]=(8+adc_channel)<<4;
				ch6buffer[2]=0;
				
				wiringPiSPIDataRW(spi_channel, ch6buffer, 3);
				
				adc_value = ((ch6buffer[1])<<8)+ch6buffer[2];
				volt = ((ch6buffer[1]&3)<<8)+ch6buffer[2];
				if ( flag >= 1 )
				{
					printf("ch6 value: %4d voltage: %d\n", adc_value, 5*volt / 1000);
					flag =0;
				}
				break;
				
			case 3:
				adc_channel=7;
				
				ch7buffer[0]=1;
				ch7buffer[1]=(8+adc_channel)<<4;
				ch7buffer[2]=0;
				
				wiringPiSPIDataRW(spi_channel, ch7buffer, 3);
				
				adc_value = ((ch7buffer[1])<<8)+ch7buffer[2];
				volt = ((ch7buffer[1]&3)<<8)+ch7buffer[2];
				if ( flag >= 1 )
				{
					printf("ch7 value: %4d voltage: %d\n", adc_value, 5*volt / 1000);
					flag =0;
				}
				break;
				
			default:
			break;
		}
	}
	return 0;
}
