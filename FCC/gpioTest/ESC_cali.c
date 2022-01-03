#include<stdio.h>
#include<pigpio.h>
#include<wiringPi.h>

int i;

int duty, f, rr, r, b, output;
int c=50;
int a;
int d;

int main()
{
	gpioCfgClock (2, 0, 0);
	gpioInitialise();

	gpioSetMode(18, PI_OUTPUT);
	
	gpioSetMode(17, PI_INPUT);
	gpioSetMode(27, PI_INPUT);
	gpioSetMode(22, PI_INPUT);
	gpioSetMode(23, PI_INPUT);
	
	gpioSetPWMrange(18,200);
	gpioPWM(18, c);
	gpioSetPWMfrequency( 18, 250 );
	
	f = gpioGetPWMfrequency(18);
	duty = 1250;
	printf(" pulsewidth : %dus\n", duty );
	printf("freq : %d\n", f);
	output = c;
	printf(" output : %d per \n", output );
	
	if(wiringPiSetup()==-1)
	return -1;
	
	while(1)
	{
		b++;
		if ( gpioRead(17) == 1 )
		{
			c=100;
			gpioPWM( 18, c);
			duty = 1250;
			printf(" pulsewidth : %dus \n", duty );
			output = c;
			printf(" output : %d per \n", output );
	
			delay(250);
		}
		
		else if ( gpioRead(27) == 1 )
		{
			c=1;
			gpioPWM( 18, c);
			output = c;
			printf(" output : %d per \n", output );
			
			delay(250);
		}
		
		else if ( gpioRead(22) == 1 )
		{
			c=50;
			gpioPWM( 18, c);
			output = c;
			printf(" output : %d per \n", output );
			
			delay(250);
		}
		
		else if ( gpioRead(23) == 1 )
		{
			c=50;
			gpioPWM( 18, c);
			output = c;
			printf(" output : %d per \n", output );
			
			delay(250);
		}
		
	}
	gpioTerminate();	
	return 0;
}
