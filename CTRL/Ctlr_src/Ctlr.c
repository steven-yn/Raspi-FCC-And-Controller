#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <wiringPi.h>
#include <stdint.h>
#include <wiringPiSPI.h>
#include <math.h>

int up, down, left, right, std = 0;
int cnt, flag, throttle, rolling, pitching, yawing;
int spi_channel=0;
int spi_speed=1000000;
int adc_channel;
unsigned char ch0buffer[3], ch1buffer[3], ch6buffer[3], ch7buffer[3];
int ch0_value, ch1_value, ch6_value, ch7_value;
int ch0volt, ch1volt, ch6volt, ch7volt;
long preus;
int buf_cnt, cnt, flag;
struct sockaddr_rc loc_addr1 = { 0 }, rem_addr1 = { 0 };
char buf[1024] = { 0 };
int s1, client1= 0;
unsigned int opt1 = sizeof(rem_addr1);
int c=50;

void ch0SPI()
{
	adc_channel=0;

	ch0buffer[0]=1;
	ch0buffer[1]=(8+adc_channel)<<4;
	ch0buffer[2]=0;
					
	wiringPiSPIDataRW(spi_channel, ch0buffer, 3);
					
	ch0_value = ((ch0buffer[1])<<8)+ch0buffer[2];
	ch0volt = ((ch0buffer[1]&3)<<8)+ch0buffer[2];
	throttle = 5*ch0volt / 1000;
					
	if ( flag >= 1 )
	{
		//printf("ch0 value: %4d voltage: %d\n", ch0_value, throttle);
		flag =0;
	}
}

void ch1SPI()
{
	adc_channel=1;
					
	ch1buffer[0]=1;
	ch1buffer[1]=(8+adc_channel)<<4;
	ch1buffer[2]=0;
					
	wiringPiSPIDataRW(spi_channel, ch1buffer, 3);
					
	ch1_value = ((ch1buffer[1])<<8)+ch1buffer[2];
	ch1volt = ((ch1buffer[1]&3)<<8)+ch1buffer[2];
					
	yawing = 5*ch1volt / 1000;
					
	if ( flag >= 1 )
	{
		//printf("ch1 value: %4d voltage: %d\n", ch1_value, yawing);
		flag =0;
	}
}

void ch6SPI()
{
	adc_channel=6;
					
	ch6buffer[0]=1;
	ch6buffer[1]=(8+adc_channel)<<4;
	ch6buffer[2]=0;
					
	wiringPiSPIDataRW(spi_channel, ch6buffer, 3);
					
	ch6_value = ((ch6buffer[1])<<8)+ch6buffer[2];
	ch6volt = ((ch6buffer[1]&3)<<8)+ch6buffer[2];
	pitching = 5*ch6volt / 1000;
					
	if ( flag >= 1 )
	{
		//printf("ch6 value: %4d voltage: %d\n", ch6_value, pitching);
		flag =0;
	}
}

void ch7SPI()
{
	adc_channel=7;
					
	ch7buffer[0]=1;
	ch7buffer[1]=(8+adc_channel)<<4;
	ch7buffer[2]=0;
					
	wiringPiSPIDataRW(spi_channel, ch7buffer, 3);
					
	ch7_value = ((ch7buffer[1])<<8)+ch7buffer[2];
	ch7volt = ((ch7buffer[1]&3)<<8)+ch7buffer[2];
	rolling = 5*ch7volt / 1000;
					
	if ( flag >= 1 )
	{
		//printf("ch7 value: %4d voltage: %d\n", ch7_value, rolling);
		flag =0;
	}
}

void transmitbuf ()
{
	if( read(client1, buf, 1024) > 0 )
	{

			if( yawing == 5 )
			{
				write(client1,"12",2);
			}
			else if ( yawing == 0 )
			{
				write(client1,"10",2);
			}
				
			else if( throttle == 5 )
			{ 
				write(client1,"02",2);
			}
			else if ( throttle == 0 )
			{
				write(client1,"00",2);
			}
				
			else if( pitching == 5 )
			{
				write(client1,"62",2);
			}
			else if ( pitching == 0 )
			{
				write(client1,"60",2);
			}
				
				
			else if( rolling == 5 )
			{
				write(client1,"72",2);
			}
			else if ( rolling == 0 )
			{
				write(client1,"70",2);
			}
				
			else
			{
				write(client1,"01",2);
			}
		
	}
}

int main()
{
	if(wiringPiSetup()==-1)
	return -1;
	
	if(wiringPiSPISetup( spi_channel,spi_speed )==-1)
	return-1;
	
	// allocate socket
	s1 = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	printf("[make server socket]\n");

	// bind socket to port 1 of the first available 
	// local bluetooth adapter

	loc_addr1.rc_family = AF_BLUETOOTH;
	loc_addr1.rc_bdaddr = *BDADDR_ANY;
	loc_addr1.rc_channel = (uint8_t) 1;
	bind(s1, (struct sockaddr *)&loc_addr1, sizeof(loc_addr1));
	printf("[bind socket]\n");

	// put socket into listening mode
	listen(s1, 1);
	printf("[listen to client]\n");
	
	while(1)
	{
		client1 = accept(s1, (struct sockaddr *)&rem_addr1, &opt1);
		
		if (client1 == -1)
		{
			perror("Accept error\n");
			close(client1);
			close(s1);
			return 0;
			break;
		}
		
		printf("[accept client]\n");
		
		while(1)
		{
			unsigned long curus = millis(); // each 1ms ++curr~
			if(curus - preus > 10) // more than 10ms
			{
				preus = curus; // each 1ms pre~ += 10
				if ( cnt++ >= 4 ) // 40ms
				{
					cnt = 0;
				}
				flag++;
				buf_cnt++;
			}
		
			switch (cnt)
			{
				case 0:
					ch0SPI();
					break;
			
				case 1:
					ch1SPI();
					break;
				
				case 2:
					ch6SPI();				
					break;
					
				case 3:
					ch7SPI();
					break;
					
				default:
				break;
			}
			
			transmitbuf();
		}
	return 0;
	}
}
