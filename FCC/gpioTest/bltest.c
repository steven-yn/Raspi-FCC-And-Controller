#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<bluetooth/bluetooth.h>
#include<bluetooth/rfcomm.h>
#include<wiringPi.h>
#include<stdint.h>

long prems = 0;
long interval = 1000;
char cnt2 =0, cnt1=0;
int up, down, left, right, std = 0;


int main()
{
	struct sockaddr_rc loc_addr1 = { 0 }, rem_addr1 = { 0 };
	char buf[1024] = { 0 };
	int s1, client1= 0;
	unsigned int opt1 = sizeof(rem_addr1);

	if(wiringPiSetup()==-1)
	return -1;
	
	pinMode(29,INPUT);//Left
	pinMode(28,INPUT);//Right
	pinMode(25,INPUT);//Up
	pinMode(24,INPUT);//Down
	
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
			left = digitalRead(29);
			right = digitalRead(28);
			up = digitalRead(25);
			down = digitalRead(24);
			
			if( up + down + left + right == 0 )
			std = 1;
			
			unsigned long curms = millis(); // each 1ms ++curr~
			if(curms - prems > interval) // more than 1000ms
			{
				prems = curms; // each 1s pre~ += 1000
				cnt1++;
				cnt2++;
				if (cnt1 >= 2)
				{
					printf("LED status : [%s]\n", buf);
					cnt1 = 0;
				}
				
				if (cnt2 >= 7)
				{
					printf("Connection state : good\n");
					cnt2 = 0;
				}
					
			}
			
			if( read(client1, buf, 1024) > 0 )
			{
				if( up == HIGH )
				{
					write(client1,"1",1);
					left = 0;
					right = 0;
					down = 0;
					std = 0;
				}
				else if( down == HIGH )
				{
					write(client1,"2",1);
					left = 0;
					right = 0;
					up = 0;
					std = 0;
				}
				else if( left == HIGH )
				{
					write(client1,"3",1);
					down = 0;
					right = 0;
					up = 0;
					std = 0;
				}
				else if( right == HIGH )
				{
					write(client1,"4",1);
					left = 0;
					down = 0;
					up = 0;
					std = 0;
				}
				else if( std == 1 )
				{
					write(client1,"0",1);
				}
			}
		}
	}
}
		/*else
		{
			ba2str( &rem_addr1.rc_bdaddr, buf );
			fprintf(stderr, "accepted connection from %s\n", buf);
			memset(buf, 0, sizeof(buf));
	
			// read data from the client
			
		}*/
		
		/*if( read(client1, buf, 1024) > 0 )
		{
			write(client1, "4", 1);
			printf("received [%s]\n", buf);
			memset(buf,0,sizeof(buf));
		}*/
		
		/*if
		
		*/	
		/*unsigned long curms = millis(); // each 1ms ++curr~
		if(curms - prems > interval) // more than 1000ms
		{
			prems = curms; // each 1s pre~ += 1000
			cnt++;
			wcnt++;
			if (wcnt >= 2)
			{
				printf("buffer : [%s]\n", buf);
				printf("writing data...\n");
				if( write(client1,"4",2) > 0 )
				{
					wcnt = 0;
				}
			}
			
			if (cnt >= 180) // if cnt more than 10s
			{
				// close connection
				close(client1);
				close(s1);
				break;
				return 0; // exit process
			
			}
		}*/
			
