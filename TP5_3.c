#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <wiringPi.h>
#include <stdlib.h>

#define LED_GPIO 17

int message(int client, char *message) {
	int size = strlen(message);
	printf("Server>>>%s\n", (message+1));
	if (write(client, message, size)<0) {
		perror("Erreur: ne peut écrire sur le client\n");
		return -1;
	}
	write(client, "\n\rERPi>", 7);
	return 0;
}

int processCommand(int client, char *command) {
	int val = -1;
	if (strcmp(command, "LED on")==0) {
		val = message(client, "\r[Turning the led on]");
		digitalWrite(LED_GPIO, HIGH);
	}	
	else if(strcmp(command, "LED off")==0) {
			val = message(client, "\r[Turning the led off]");
			digitalWrite(LED_GPIO, LOW);
	}
	else if(strcmp(command, "quit")==0) {
		val = message(client, "\r[Arrêt]");
	}
	else {
		val = message(client, "\r[Erreur : commande inconnue]");
	}
	return val;
}

int main(int argc, char **argv) {
	int client, count = 0;
	unsigned char c;
	char *command = malloc(255);
	wiringPiSetupGpio();
	pinMode(LED_GPIO, OUTPUT);
	
	if ((client = open("/dev/ttyAMA0", O_RDWR|O_NOCTTY|O_NDELAY))<0) {
		perror("UART : ne peut ouvrir le fichier.\n");
		return -1;
	}
	struct termios options;
	tcgetattr(client,&options);
	options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL;
	tcflush(client,TCIFLUSH);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	tcsetattr(client,TCSANOW,&options);
	if (message(client, "\n\rRPi Serial Server Running")<0){
	perror("UART : erreur démarrage serveur\n");
	return -1;
	}
	do {
		if (read(client, &c, 1)>0) {
			write(STDOUT_FILENO, &c, 1);
			command[count++]=c;
			if (c=='\n') {
				command[count-1]='\0';
				processCommand(client, command);
				count = 0;
			}
		}
		if (read(STDIN_FILENO, &c, 1)>0) {
			write(client, &c, 1);
		}
	}
	while(strcmp(command, "quit")!=0);
	close(client);
	return 0;
}
