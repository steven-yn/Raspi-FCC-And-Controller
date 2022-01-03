#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <pigpio.h>
#include <wiringPiI2C.h>
#include "imu_bno.h"
#include <wiringPi.h>

short GyX, GyY, GyZ, AcX, AcY, AcZ, bfGyX, bfGyY;
unsigned char leftroll, rightroll, forpitch, backpitch, hovering;
int fd, output;
int c = 50;

void imu_init()
{
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
}

void imu()
{
    GyX = (wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_X_MSB_ADDR) & 0xFF) << 8;
    GyX |= wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_X_LSB_ADDR) & 0xFF;

    AcX = (wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_X_MSB_ADDR) & 0xFF) << 8;
    AcX |= wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_X_LSB_ADDR) & 0xFF;

    GyY = (wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Y_MSB_ADDR) & 0xFF) << 8;
    GyY |= wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Y_LSB_ADDR) & 0xFF;

    AcY = (wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_X_MSB_ADDR) & 0xFF) << 8;
    AcY |= wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_X_LSB_ADDR) & 0xFF;

    GyZ = (wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Z_MSB_ADDR) & 0xFF) << 8;
    GyZ |= wiringPiI2CReadReg8(fd, BNO055_GYRO_DATA_Z_LSB_ADDR) & 0xFF;

    AcZ = (wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_Z_MSB_ADDR) & 0xFF) << 8;
    AcZ |= wiringPiI2CReadReg8(fd, BNO055_ACCEL_DATA_Z_LSB_ADDR) & 0xFF;

    //printf("Gy: X =%6d / Y =%6d\n", GyX, GyY );

    if (GyX < -950)
    {
        forpitch = 1;
        gpioPWM(18, c + 3);
        gpioPWM(17, c + 1);
        gpioPWM(27, c + 3);
        gpioPWM(22, c + 1);
        output = c;
        printf("Hovering After forpitch,,\n");
    }
    if (GyX < -570)
    {
        forpitch = 1;
        gpioPWM(18, c + 2);
        gpioPWM(17, c + 1);
        gpioPWM(27, c + 2);
        gpioPWM(22, c + 1);
        output = c;
        printf("Hovering After forpitch,,\n");
    }
    else
        forpitch = 0;

    if (GyX > 950)
    {
        backpitch = 1;
        gpioPWM(18, c + 1);
        gpioPWM(17, c + 3);
        gpioPWM(27, c + 1);
        gpioPWM(22, c + 3);
        output = c;
        printf("Hovering After forpitch,,\n");
    }
    if (GyX > 570)
    {
        backpitch = 1;
        gpioPWM(18, c + 1);
        gpioPWM(17, c + 2);
        gpioPWM(27, c + 1);
        gpioPWM(22, c + 2);
        output = c;
        printf("Hovering After forpitch,,\n");
    }
    else
        backpitch = 0;

    if (GyY > 950)
    {
        leftroll = 1;
        gpioPWM(18, c + 1);
        gpioPWM(17, c + 1);
        gpioPWM(27, c + 3);
        gpioPWM(22, c + 3);
        output = c;
        printf("Hovering After leftroll,,\n");
    }
    if (GyY > 570)
    {
        leftroll = 1;
        gpioPWM(18, c + 1);
        gpioPWM(17, c + 1);
        gpioPWM(27, c + 2);
        gpioPWM(22, c + 2);
        output = c;
        printf("Hovering After leftroll,,\n");
    }
    else
        leftroll = 0;

    if (GyY < -950)
    {
        rightroll = 1;
        gpioPWM(18, c + 3);
        gpioPWM(17, c + 3);
        gpioPWM(27, c + 1);
        gpioPWM(22, c + 1);
        output = c;
        printf("Hovering After rightroll,,\n");
    }
    if (GyY < -570)
    {
        rightroll = 1;
        gpioPWM(18, c + 2);
        gpioPWM(17, c + 2);
        gpioPWM(27, c + 1);
        gpioPWM(22, c + 1);
        output = c;
        printf("Hovering After rightroll,,\n");
    }
    else
        rightroll = 0;
}

void imucheck()
{
    if (bfGyX >= GyX)
    {
        forpitch = 0;
        backpitch = 0;
    }

    if (bfGyY >= GyY)
    {
        rightroll = 0;
        leftroll = 0;
    }

    bfGyX = GyX;
    bfGyY = GyY;

    if ((forpitch == 0) && (backpitch == 0) && (leftroll == 0) && (rightroll == 0))
    {
        hovering = 1;
        gpioPWM(18, c);
        gpioPWM(17, c);
        gpioPWM(27, c);
        gpioPWM(22, c);
        output = c;
        printf(" output : %d per \n", output);
    }
    else
    {
        hovering = 0;
    }
}