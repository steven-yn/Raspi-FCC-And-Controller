#include <stdio.h>
#include <pigpio.h>
#include <wiringPi.h>
#include <stdint.h>
#include <wiringPiI2C.h>

#include "imu_bno.h"

int i, a, d;
int duty, f, rr, r, b, output;
int c = 50;
int fd, cnt1, cnt2;
unsigned char leftroll, rightroll, forpitch, backpitch;
const int bno055_addr = 0x28;
short GyX,GyY,GyZ;
long prems = 0;

void imu()
{
	GyX = (wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_X_MSB_ADDR) & 0xFF) << 8;
	GyX |= wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_X_LSB_ADDR) & 0xFF;
			
	GyY = (wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Y_MSB_ADDR) & 0xFF) << 8;
	GyY |= wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Y_LSB_ADDR) & 0xFF;
			
	GyZ = (wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Z_MSB_ADDR) & 0xFF) << 8;
	GyZ |= wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Z_LSB_ADDR) & 0xFF;
			
	printf("Gy: X =%6d / Y =%6d\n", GyX, GyY );
			
	if ( GyX < -500 )
	{
		digitalWrite( 24, 1 ); // left gyro
		digitalWrite( 28, 0 );
		leftroll = 1;	
		rightroll = 0;
		forpitch = 0;
		backpitch = 0;
	}
	else if ( GyX > 500 )
	{
		digitalWrite( 28, 1 ); // right gyro
		digitalWrite( 24, 0 ); 
		rightroll = 1;
		leftroll = 0;
		forpitch = 0;
		backpitch = 0;
	}
	else
	{
		digitalWrite( 24, 0 );
		digitalWrite( 28, 0 );
	}
			
	if ( GyY > 500 )
	{
		digitalWrite( 21, 1 ); // forward gyro
		digitalWrite( 22, 0 );
		forpitch = 1;
		backpitch = 0;
		leftroll = 0;	
		rightroll = 0;
	}
	else if ( GyY < -500 )
	{
		digitalWrite( 22, 1 ); // back gyro
		digitalWrite( 0, 0 );
		backpitch = 1;
		forpitch = 0;
		leftroll = 0;	
		rightroll = 0;
	}
	else
	{
		digitalWrite( 21, 0 );
		digitalWrite( 22, 0 );
	}
}

void esc()
{		
	if ( leftroll == 1 ) // roll ( left )
	{
		gpioPWM( 18, c+2);
		gpioPWM( 17, c+2);
		gpioPWM( 27, c-2);
		gpioPWM( 22, c-2);
		c = c;
		output = c;
		//printf(" leftroll : %d per \n", output );
	}		
	else if ( rightroll == 1 ) // roll ( right )
	{
		gpioPWM( 18, c-2);
		gpioPWM( 17, c-2);
		gpioPWM( 27, c+2);
		gpioPWM( 22, c+2);
		c = c;
		output = c;
		//printf(" rightroll : %d per \n", output );
	}
	/*else
	{
		c=c;
		gpioPWM( 18, c);
		gpioPWM( 17, c);
		gpioPWM( 27, c);
		gpioPWM( 22, c);
		output = c;
		//printf(" output : %d per \n", output );
	}	*/
	
	else if ( forpitch == 1 ) // pitch ( forward )
	{
		gpioPWM( 18, c+2);
		gpioPWM( 17, c-2);
		gpioPWM( 27, c+2);
		gpioPWM( 22, c-2);
		c=c;
		output = c;
		//printf(" forpitch : %d per \n", output );
	}
				
	else if ( backpitch == 1 ) // pitch ( back )
	{
		gpioPWM( 18, c-2);
		gpioPWM( 17, c+2);
		gpioPWM( 27, c-2);
		gpioPWM( 22, c+2);
		c=c;
		output = c;
		//printf(" backpitch : %d per \n", output );
	}
	
	else
	{
		c=c;
		gpioPWM( 18, c);
		gpioPWM( 17, c);
		gpioPWM( 27, c);
		gpioPWM( 22, c);
		output = c;
		//printf(" output : %d per \n", output );
	}
}

int main()
{
	wiringPiSetup();
	
	pinMode(21,OUTPUT); //pitching gyro (forward)
	pinMode(23,OUTPUT); //pitching accel 
	pinMode(22,OUTPUT); //pitching gyro (back)
	pinMode(26,OUTPUT); //pitching accel
	pinMode(24,OUTPUT); //rolling gyro (left)
	pinMode(25,OUTPUT); //rolling accel
	pinMode(28,OUTPUT); //rolling gyro (right)
	pinMode(27,OUTPUT); //rolling accel
	
	fd = wiringPiI2CSetup(bno055_addr);
	if ( fd<0 )
		return -1;
	
	pullUpDnControl (9, PUD_DOWN);
	delay(1);
	pullUpDnControl (9, PUD_UP);
	
	/* Reset */
	wiringPiI2CWriteReg8(fd, BNO055_SYS_TRIGGER_ADDR, 0x20);
	delay(30); // 30ms
	
	while (wiringPiI2CReadReg8(fd, BNO055_CHIP_ID_ADDR) != BNO055_ID) 
	{
		delay(10); // 10ms
	}
	delay(50); // 50ms
	
	/* Set to normal power mode */
	wiringPiI2CWriteReg8(fd, BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
	delay(10);
	
	wiringPiI2CWriteReg8(fd, BNO055_PAGE_ID_ADDR, 0);
	
	wiringPiI2CWriteReg8(fd, BNO055_SYS_TRIGGER_ADDR, 0x0);
	delay(10);
	
	/* Set the requested operating mode */
	wiringPiI2CWriteReg8(fd, BNO055_OPR_MODE_ADDR, OPERATION_MODE_AMG);
	delay(30);
	gpioCfgClock (2, 0, 0);
	gpioInitialise();

	gpioSetMode(18, PI_OUTPUT); //s4
	gpioSetMode(17, PI_OUTPUT); //s3
	gpioSetMode(27, PI_OUTPUT); //s2
	gpioSetMode(22, PI_OUTPUT); //s1
	
	gpioSetPWMrange(18,200);
	gpioSetPWMrange(27,200);
	gpioSetPWMrange(22,200);
	gpioSetPWMrange(17,200);
	
	gpioPWM(18, c);
	gpioPWM(27, c);
	gpioPWM(22, c);
	gpioPWM(17, c);
	
	gpioSetPWMfrequency( 18, 250 );
	gpioSetPWMfrequency( 27, 250 );
	gpioSetPWMfrequency( 22, 250 );
	gpioSetPWMfrequency( 17, 250 );
	
	f = gpioGetPWMfrequency(18);
	f = gpioGetPWMfrequency(27);
	f = gpioGetPWMfrequency(22);
	f = gpioGetPWMfrequency(17);
	
	duty = 1250;
	
	while(1)
	{
		unsigned long curms = millis(); // each 1ms ++curr~
		if(curms - prems > 50) // more than 200ms
		{
			prems = curms; // each 1s pre~ += 200
			cnt1++;
			cnt2++;
		}
		
		if ( cnt1 >= 1 ) // 50ms
		{
			imu();
			cnt1=0;
		}
		
		if ( cnt2 >= 4 ) // 200ms
		{
			esc();
			cnt2=0;
		}
	}
}
