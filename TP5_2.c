#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

int main(int argc, char *argv[]){
	int file, count;
	if(argc!=2){
		printf("Passer une chaine en argument, sortie\n");
		return -2;
	}

if((file=open("/dev/ttyS0",O_RDWR | O_NOCTTY | O_NDELAY))<0){
	perror("Erreur d'ouverture de l'UART.\n");
	return -1;
}
struct termios options;
tcgetattr(file,&options);
options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
options.c_iflag = IGNPAR | ICRNL;
tcflush(file,TCIFLUSH);
tcsetattr(file,TCSANOW,&options);
if ((count=write(file,argv[1], strlen(argv[1])))<0){
	perror("UART : erreur écriture\n");
	return -1;
}
write(file, "\n\r",2);
close(file);
return 0;
}
