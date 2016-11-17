#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#define ID_LENTH 12
struct{
	int userID[ID_LENTH];
	int returnFlag;
	char *name;
}User[] = {	
	{{4,12,2,32,0,4,0,101,244,8,137,193},0,"Ben"},
	{{4,12,2,32,0,2,0,122,45,162,231,197},0,"Simon"}
};
int j;
int idAuth(int *testID) {
	int i,k = 0;
	for(j = 0; j < 2; j++) {
		for(i = 0; i < ID_LENTH; i++) {
			if(*(User[j].userID+i) != *(testID+i)) {
				k = 0;
				break;
			}
			k++;
		}
		if(k == 12) 
			break;
	}
	if(k == 12)
		return j;
	else
		return -1;
}

int main(){
	int fd;
	char data[] = {0x03,0x08,0xC1,0x20,0x02,0x00,0x00,0x17};
	int readData;
	int ID[12];
	int i;
	struct timeval tpstart[2],tpend[2];
	unsigned long diff; 
	wiringPiSetup();
	pinMode(0,OUTPUT);
	pinMode(2,INPUT);
	fd = serialOpen("/dev/ttyAMA0",9600);
	if(fd < 0) return 1;
	system("clear");
	printf("System is Running...\n");
	serialPuts(fd,data);
	while(1) {
		if(digitalRead(2) == 1) {
			for(i = 0; i < ID_LENTH; i++) {
				readData = serialGetchar(fd);
				*(ID+i) = readData;
			}

			if(idAuth(ID) == -1){
				system("clear");
				printf("Permission denied!\n");
				digitalWrite(0,LOW);
				delay(500);
			}
			else{
				if(User[idAuth(ID)].returnFlag == 0){
					gettimeofday(&tpstart[idAuth(ID)],NULL);
					system("clear");
					printf("Take Your Umbrella!,%s\n",User[idAuth(ID)].name);
					User[idAuth(ID)].returnFlag = 1;	
					digitalWrite(0,HIGH);
					delay(500);
					digitalWrite(0,LOW);
				}
				else{
					gettimeofday(&tpend[idAuth(ID)],NULL);
					diff = (tpend[idAuth(ID)].tv_sec-tpstart[idAuth(ID)].tv_sec);
					system("clear");
					printf("%s:have a nice day!,time= %lu Sec\n",User[idAuth(ID)].name,diff);
					delay(500);
					User[idAuth(ID)].returnFlag = 0;
				}
			}
		}	
	}
	serialClose(fd);
	return 0;
}

