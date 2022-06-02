#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>

int main()
{
	char cr;
	int fd;
	if(wiringPiSetup() < 0)return 1;
	if((fd=serialOpen("/dev/ttyS0",115200)) < 0)return 1;
	printf("uart\n");
	serialPrintf(fd,"Hello\n");
	while(1)
	{if (serialDataAvail(fd)>0)
		{cr=serialGetchar(fd);
			serialPutchar(fd,cr);
			printf("%c",cr);
		}
	}
	serialClose(fd);
	return 0;
}
