#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <wiringPi.h>
#include <stdint.h>
#include <stdlib.h>
#include <pigpio.h>
#include <wiringPiI2C.h>
#include "IMU.h"

long prems = 0;
struct sockaddr_rc addr1 = {0};
char buf[1024] = {0};
int s1, i, a, d;
int duty, f, rr, r, b, output;
int c = 50;
int motorout = 0;
int fd, cnt50ms, cnt100ms, cnt200ms;
unsigned char reset = 1;
const int bno055_addr = 0x28;

void joystick()
{
	switch (motorout)
	{
		/*case 01 :			// middle
			gpioPWM( 18, c);
			gpioPWM( 17, c);
			gpioPWM( 27, c);
			gpioPWM( 22, c);
			output = c;
			printf(" output : %d per \n", output );
			break;*/

	case 02: // up
		c++;
		gpioPWM(18, c);
		gpioPWM(17, c);
		gpioPWM(27, c);
		gpioPWM(22, c);
		output = c;
		printf(" output : %d per \n", output);
		break;

	case 00: // down
		c--;
		gpioPWM(18, c);
		gpioPWM(17, c);
		gpioPWM(27, c);
		gpioPWM(22, c);
		output = c;
		printf(" output : %d per \n", output);
		break;

		/*case 11 :
			gpioPWM( 18, c);
			gpioPWM( 17, c);
			gpioPWM( 27, c);
			gpioPWM( 22, c);
			output = c;
			printf(" output : %d per \n", output );	
			break;*/

	case 12: // right
		c = c + 7;
		gpioPWM(18, c);
		gpioPWM(17, c);
		gpioPWM(27, c);
		gpioPWM(22, c);
		output = c;
		printf(" output : %d per \n", output);
		break;

	case 10: // left
		c = c - 3;
		gpioPWM(18, c);
		gpioPWM(17, c);
		gpioPWM(27, c);
		gpioPWM(22, c);
		output = c;
		printf(" output : %d per \n", output);
		break;

		/*case 61 :
			gpioPWM( 18, c);
			gpioPWM( 17, c);
			gpioPWM( 27, c);
			gpioPWM( 22, c);
			output = c;
			printf(" output : %d per \n", output );
			break;*/

	case 60: // roll ( left )
		gpioPWM(18, c + 2);
		gpioPWM(17, c + 2);
		gpioPWM(27, c - 2);
		gpioPWM(22, c - 2);
		output = c;
		printf(" leftroll : %d per \n", output);
		leftroll = 1;
		rightroll = 0;
		cnt100ms = 0;
		break;

	case 62: // roll ( right )
		gpioPWM(18, c - 2);
		gpioPWM(17, c - 2);
		gpioPWM(27, c + 2);
		gpioPWM(22, c + 2);
		output = c;
		printf(" rightroll : %d per \n", output);
		rightroll = 1;
		leftroll = 0;
		cnt100ms = 0;
		break;

		/*case 71 :
			gpioPWM( 18, c);
			gpioPWM( 17, c);
			gpioPWM( 27, c);
			gpioPWM( 22, c);
			output = c;
			printf(" output : %d per \n", output );
			break;*/

	case 72: // pitch ( forward )
		gpioPWM(18, c - 2);
		gpioPWM(17, c + 2);
		gpioPWM(27, c - 2);
		gpioPWM(22, c + 2);
		output = c;
		printf(" forpitch : %d per \n", output);
		forpitch = 1;
		backpitch = 0;
		cnt100ms = 0;
		break;

	case 70: // pitch ( back )
		gpioPWM(18, c + 2);
		gpioPWM(17, c - 2);
		gpioPWM(27, c + 2);
		gpioPWM(22, c - 2);
		output = c;
		printf(" backpitch : %d per \n", output);
		backpitch = 1;
		forpitch = 0;
		cnt100ms = 0;
		break;

	default:
		break;
	}
}

int main(int argc, char **argv)
{
	if (wiringPiSetup() == -1)
		return -1;

	fd = wiringPiI2CSetup(bno055_addr);

	if (fd < 0)
		return -1;

	pullUpDnControl(9, PUD_DOWN);
	delay(1);
	pullUpDnControl(9, PUD_UP);

	imu_init();

	char dest[18] = "B8:27:EB:51:C5:7A";

	// allocate a socket
	s1 = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	printf("[make client socket]\n");

	// set the connection parameters (who to connect to)
	addr1.rc_family = AF_BLUETOOTH;
	addr1.rc_bdaddr = *BDADDR_ANY;
	addr1.rc_channel = (uint8_t)1;
	str2ba(dest, &addr1.rc_bdaddr);

	// connect to server
	printf("[connect to server]\n");

	// unconnect to server
	if (connect(s1, (struct sockaddr *)&addr1, sizeof(addr1)) < 0)
	{
		perror("uh oh");
		close(s1);
		return 0;
	}

	// send a message
	printf("message send to server\n");

	gpioCfgClock(2, 0, 0);
	gpioInitialise();

	gpioSetMode(18, PI_OUTPUT); //s4
	gpioSetMode(17, PI_OUTPUT); //s3
	gpioSetMode(27, PI_OUTPUT); //s2
	gpioSetMode(22, PI_OUTPUT); //s1

	gpioSetPWMrange(18, 200);
	gpioSetPWMrange(27, 200);
	gpioSetPWMrange(22, 200);
	gpioSetPWMrange(17, 200);

	gpioPWM(18, c);
	gpioPWM(27, c);
	gpioPWM(22, c);
	gpioPWM(17, c);

	gpioSetPWMfrequency(18, 250);
	gpioSetPWMfrequency(27, 250);
	gpioSetPWMfrequency(22, 250);
	gpioSetPWMfrequency(17, 250);

	f = gpioGetPWMfrequency(18);
	f = gpioGetPWMfrequency(27);
	f = gpioGetPWMfrequency(22);
	f = gpioGetPWMfrequency(17);

	duty = 1250;
	printf(" pulsewidth : %dus\n", duty);
	printf("freq : %d\n", f);
	output = c;
	printf(" output : %d per \n", output);

	while (1)
	{
		unsigned long curms = millis(); // each 1ms ++curr~
		if (curms - prems > 5)			// more than 1000ms
		{
			prems = curms; // each 1s pre~ += 1000
			cnt50ms++;
			cnt100ms++;
			cnt200ms++;
		}

		write(s1, buf, 1);
		read(s1, buf, 1024);

		motorout = atoi(buf);

		if (cnt50ms >= 10)
		{
			imucheck();
			if (motorout == 01)
			{
				imu();
			}
			cnt50ms = 0;
		}

		if (cnt100ms >= 20)
		{

			cnt100ms = 0;
		}

		if (cnt200ms >= 40)
		{
			joystick();
			cnt200ms = 0;
		}
	}
}
