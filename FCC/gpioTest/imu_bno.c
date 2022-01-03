#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>

#include "imu_bno.h"
int main(void)
{
	int fd, cnt;
	unsigned char leftroll, rightroll, forpitch, backpitch;
	const int bno055_addr = 0x28;
	short GyX,GyY,GyZ, AcX,AcY,AcZ, MaX,MaY,MaZ, GrX,GrY,GrZ;
	short bfAcX,bfAcY,bfAcZ;
	
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
	
	while(1)
	{
		GyX = (wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_X_MSB_ADDR) & 0xFF) << 8;
		GyX |= wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_X_LSB_ADDR) & 0xFF;
		
		AcX = (wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_X_MSB_ADDR) & 0xFF) << 8;
		AcX |= wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_X_LSB_ADDR) & 0xFF;

		MaX = (wiringPiI2CReadReg8(fd, BNO055_MAG_DATA_X_MSB_ADDR) & 0xFF) << 8;
		MaX |= wiringPiI2CReadReg8(fd, BNO055_MAG_DATA_X_LSB_ADDR) & 0xFF;
		
		GyY = (wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Y_MSB_ADDR) & 0xFF) << 8;
		GyY |= wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Y_LSB_ADDR) & 0xFF;
		
		AcY = (wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_X_MSB_ADDR) & 0xFF) << 8;
		AcY |= wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_X_LSB_ADDR) & 0xFF;
		
		MaY = (wiringPiI2CReadReg8(fd, BNO055_MAG_DATA_Y_MSB_ADDR) & 0xFF) << 8;
		MaY |= wiringPiI2CReadReg8(fd, BNO055_MAG_DATA_Y_LSB_ADDR) & 0xFF;
		
		GyZ = (wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Z_MSB_ADDR) & 0xFF) << 8;
		GyZ |= wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Z_LSB_ADDR) & 0xFF;
		
		AcZ = (wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_Z_MSB_ADDR) & 0xFF) << 8;
		AcZ |= wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_Z_LSB_ADDR) & 0xFF;
		
		MaZ = (wiringPiI2CReadReg8(fd, BNO055_MAG_DATA_Z_MSB_ADDR) & 0xFF) << 8;
		MaZ |= wiringPiI2CReadReg8(fd, BNO055_MAG_DATA_Z_LSB_ADDR) & 0xFF;
		
		printf("Ac: X =%6d / Y =%6d\n", AcX, AcY );
		printf("Gy: X =%6d / Y =%6d\n", GyX, GyY );
		//printf("Ac: X =%6d bfX =%6d / Y =%6d bfYbk =%6d / Z =%6d \n", AcX, bfAcX, AcY, bfAcYback, AcZ );
		
		if ( GyX < -800 )
		{
			digitalWrite( 24, 1 ); // left gyro
			digitalWrite( 28, 0 );
			leftroll = 1;
			rightroll = 0;
		}
		else if ( GyX > 800 )
		{
			digitalWrite( 28, 1 ); // right gyro
			digitalWrite( 24, 0 ); 
			rightroll = 1;
			leftroll = 0;
		}
		else
		{
			digitalWrite( 24, 0 );
			digitalWrite( 28, 0 );
		}
		
		if ( GyY > 800 )
		{
			digitalWrite( 0, 1 ); // forward gyro
			digitalWrite( 22, 0 );
			forpitch = 1;
			backpitch = 0;
		}
		else if ( GyY < -800 )
		{
			digitalWrite( 22, 1 ); // back gyro
			digitalWrite( 0, 0 );
			backpitch = 1;
			forpitch = 0;
		}
		else
		{
			digitalWrite( 0, 0 );
			digitalWrite( 22, 0 );
		}
		
		
		delay(50); // 50ms
	}
}
