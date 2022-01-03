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
