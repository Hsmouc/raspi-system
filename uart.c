#include <stdio.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#define ID_LENTH 12
typedef int BOOL;
BOOL idAuth(int *testID) {
	int i;
	int userID[ID_LENTH] = {4,12,2,32,0,4,0,101,244,8,137,193};
	for(i = 0; i < ID_LENTH; i++) {
		if(*(userID+i) != *(testID+i)) {
			return FALSE;
		}
	}
	return TRUE;
}
void UI(void) {
	printf("     ****     	  **	  **	     *****	\n");
	printf("  **      **  	  **	  **       **     	\n");
	printf(" **        ** 	  **	  **	  **	 	\n");
	printf(" **        ** 	  **	  **	  **	 	\n");
	printf(" **        ** 	  **	  **	  **	 	\n");
	printf("  **	  **	   **    ** 	   **           \n");
	printf("     ****	     ****	     *****      \n");
}

int main(){
	int fd;
	char data[] = {0x03,0x08,0xC1,0x20,0x02,0x00,0x00,0x17};
	int readData;
	int ID[12];
	int i;
	UI();
	wiringPiSetup();
	pinMode(0,OUTPUT);
	fd = serialOpen("/dev/ttyAMA0",9600);
	if(fd < 0) return 1;
	printf("Serial Test Start...\n");
	serialPuts(fd,data);
	while(1) {
		for(i = 0; i < ID_LENTH; i++) {
			readData = serialGetchar(fd);
			*(ID+i) = readData;
		}
		if(idAuth(ID) == FALSE){
			printf("ERROR ID\n");
		}
		else{
			printf("TAKE YOUR UMBRELLA\n");
			digitalWrite(0,HIGH);
			delay(500);
		}
		//for(i = 0; i < ID_LENTH; i++) {
		//	printf("%d\n",*(ID+i));
		//}
	}
	serialClose(fd);
	return 0;
}

