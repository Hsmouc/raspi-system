#include <stdio.h>
#include <sys/time.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#define ID_LENTH 12
struct User{
	int userID[ID_LENTH];
	int returnFlag;
	char *name;
}User[2];

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
	
int read_db() {  
    	MYSQL           mysql;  
    	MYSQL_RES       *res = NULL;  
    	MYSQL_ROW       row;  
    	char            *query_str = NULL;  
    	int             rc, i, j, fields;  
    	int             rows;  
    	if (NULL == mysql_init(&mysql)) {  
        	printf("mysql_init(): %s\n", mysql_error(&mysql));  
        	return -1;  
    	}  
    	if (NULL == mysql_real_connect(&mysql,  
                	"localhost",  
                	"root",  
                	"ubuntu",  
                	"umbrella_db",  
                	0,  
                	NULL,  
                	0)) {  
        	printf("mysql_real_connect(): %s\n", mysql_error(&mysql));  
        	return -1;  
    	}  
    	printf("1. Connected MySQL successful! \n");  
    	query_str = "select * from users";  
    	rc = mysql_real_query(&mysql, query_str, strlen(query_str));  
    	if (0 != rc) {  
        	printf("mysql_real_query(): %s\n", mysql_error(&mysql));  
        	return -1;  
    	}  
    	res = mysql_store_result(&mysql);  
    	if (NULL == res) {  
        	 printf("mysql_restore_result(): %s\n", mysql_error(&mysql));  
         	return -1;  
    	}  
    	rows = mysql_num_rows(res);  
    	printf("The total rows is: %d\n", rows);  
    	fields = mysql_num_fields(res);  
    	printf("The total fields is: %d\n", fields);  
 	for(j = 0; j < 2; j++) {
		row = mysql_fetch_row(res);
		for(i = 1; i < 13; i++) {
			*(User[j].userID+i-1) = atoi(row[i]);
			User[j].name = row[13];
			User[j].returnFlag = atoi(row[14]);		
		}
	}
	mysql_close(&mysql);
	return 0;
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
	delay(2000);
	if(read_db() == 0) {
		printf("Database is connected\n");
	}
	printf("Copyright OUC:Industrial Automation\n");
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
				delay(6000);
				system("clear");
				printf("Copyright OUC:Industrial Automation\n");
			}
			else{
				if(User[idAuth(ID)].returnFlag == 0){
					gettimeofday(&tpstart[idAuth(ID)],NULL);
					system("clear");
					printf("Take Your Umbrella!,%s\n",User[idAuth(ID)].name);
					User[idAuth(ID)].returnFlag = 1;	
					digitalWrite(0,HIGH);
					delay(6000);
					system("clear");
					printf("Copyright OUC:Industrial Automation\n");
					digitalWrite(0,LOW);
				}
				else{
					gettimeofday(&tpend[idAuth(ID)],NULL);
					diff = (tpend[idAuth(ID)].tv_sec-tpstart[idAuth(ID)].tv_sec);
					system("clear");
					printf("%s:have a nice day!,time= %lu Sec\n",User[idAuth(ID)].name,diff);
					delay(6000);
					system("clear");
					printf("Copyright OUC:Industrial Automation\n");
					User[idAuth(ID)].returnFlag = 0;
				}
			}
		}	
	}
	serialClose(fd);
	return 0;
}

