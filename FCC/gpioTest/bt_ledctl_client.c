#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <wiringPi.h>
#include <stdint.h>
#include <stdlib.h>

long prems = 0;

struct sockaddr_rc addr1 = { 0 };
char buf[1024] = { 0 };
int s1;
int motorout = 0;
int cnt;

void printout()
{
	if ( motorout == 01 )
		printf("buf : 01\n");
						
	if ( motorout == 02 )
		printf("buf : 02\n");
				
	if ( motorout == 00 )
		printf("buf : 00\n");
			
	if ( motorout == 11 )
		printf("buf : 11\n");
					
	if ( motorout == 12 )
		printf("buf : 12\n");
				
	if ( motorout == 61 )
		printf("buf : 61\n");
				
	if ( motorout == 62 )
		printf("buf : 62\n");
				
	if ( motorout == 60 )
		printf("buf : 60\n");
				
	if ( motorout == 71 )
		printf("buf : 71\n");
				
	if ( motorout == 72 )
		printf("buf : 72\n");
				
	if ( motorout == 70 )
		printf("buf : 70\n");
}

int main(int argc, char **argv)
{

	if(wiringPiSetup()==-1)
	return -1;
	
	
	// NEXT DONGLE
	//char dest[18] = "00:1A:7D:DA:71:14";
	// Note10

	char dest[18] = "B8:27:EB:1A:A7:34";

	// allocate a socket
	s1 = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	printf("[make client socket]\n");

	// set the connection parameters (who to connect to)
	addr1.rc_family = AF_BLUETOOTH;
	addr1.rc_bdaddr = *BDADDR_ANY;
	addr1.rc_channel = (uint8_t) 1;
	str2ba( dest, &addr1.rc_bdaddr );

	// connect to server
	
	// unconnect to server
	if( connect(s1, (struct sockaddr *)&addr1, sizeof(addr1)) < 0 )
	{
		perror("uh oh");
		close(s1);
		return 0;
	}
	
	// send a message
	printf("[connect to server]\n");
	/*if ( write(s1, "LED control will be start :)", 28) > 0 )
	{
		printf("message send to server\n");
	}*/
	
	while(1)
	{			
		unsigned long curms = millis(); // each 1ms ++curr~
		if(curms - prems > 100) // more than 1000ms
		{
			prems = curms; // each 1s pre~ += 1000
			cnt++;
		}
		
		write(s1, buf, 1);
		read(s1, buf, 1024);
		
		motorout = atoi(buf);
		
		if (cnt >= 1)
		{
			printout();
			cnt=0;
		}
	}
}
