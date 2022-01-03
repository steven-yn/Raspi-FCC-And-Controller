#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <math.h>

int main()
{
	int spi_channel=0;
	int spi_speed=1000000;
	int adc_channel=0;
	
	wiringPiSetup();
	if(wiringPiSPISetup( spi_channel,spi_speed)==-1)
		return-1;
		
	unsigned char buffer[3];
	int adc_value;
	//double volt;
	
	while(1)
	{
		buffer[0]=1;
		buffer[1]=(8+adc_channel)<<4;
		buffer[2]=0;
		
		wiringPiSPIDataRW(spi_channel, buffer, 3);
		
		adc_value = ((buffer[1])<<8)+buffer[2];
		//volt = ((buffer[1]&3)<<8)+buffer[2];
		printf("ADC value: %4d voltage: %f\n", adc_value, 3.3*adc_value / 1024);
		delay(100);
	}
	return 0;
}
