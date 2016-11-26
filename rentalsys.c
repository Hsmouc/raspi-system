#include <stdio.h>
#include <time.h>
#include <mysql/mysql.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#define ID_LENTH 12
MYSQL           mysql;
MYSQL_RES       *res = NULL;
MYSQL_ROW       row;
int user_num;
struct User{
	int userID[ID_LENTH];
	char *returnFlag;
	char *name;
	int time;
}User[100];

int idAuth(int *testID) {
	int i,j,k = 0;
	for(j = 0; j < user_num; j++) {
		for(i = 0; i < ID_LENTH; i++) {
			if(*(User[j].userID+i) != *(testID+i)) {
				k = 0;
				break;
			}
			k++;
		}
		if(k == ID_LENTH) 
			break;
	}
	if(k == ID_LENTH)
		return j;
	else
		return -1;
}

int timeAuth(int time_O,int time_I) {
        if(((time_I - time_O < 0 && time_I > 2) || time_I - time_O > 2) && time_O != 1000)
		return -1;
        else
                return 0;
}

int db_connect() {
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
        printf("Connected to MySQL \n");
}
	
int read_db() {  
    	char            *query_str = NULL;  
    	int             rc,i,j,fields;  
    	int             rows;  
    	db_connect();
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
    	fields = mysql_num_fields(res);
	user_num = rows;  
    	printf("The total users is: %d\n", rows);
 	for(j = 0; j < rows; j++) {
		row = mysql_fetch_row(res);
		for(i = 1; i < 13; i++) {
			*(User[j].userID+i-1) = atoi(row[i]);
			User[j].name = row[13];
			User[j].returnFlag = row[14];
			User[j].time = atoi(row[15]);	
		}
	}
	mysql_close(&mysql);
	return 0;
}

int write_db_re(int id,const char *re_state) {
	char 		*str_1 = NULL;
	char 		*str_2 = NULL;
	char 		*str;
	int             rc;
	str_1 = "UPDATE users SET return_state=";
	str_2 = " WHERE id_0=";
	char temp[10];
	db_connect();
	sprintf(temp,"%d",id+1);
	strcat(strcat(strcat(strcpy(str,str_1),re_state),str_2),temp);
	rc = mysql_real_query(&mysql, str, strlen(str));
        if (0 != rc) {
                printf("mysql_real_query(): %s\n", mysql_error(&mysql));
                return -1;
        }
	mysql_close(&mysql);
	return 0;
}

int write_db_tm(int id,int time) {
	char		*str_1 = NULL;
	char		*str_2 = NULL;
	char 		*str;
	int 		rc;
	str_1 = "UPDATE users SET time=";
	str_2 = " WHERE id_0=";
	char temp[10];
	char tempNum[10];
	db_connect();
	sprintf(temp,"%d",id+1);
	sprintf(tempNum,"%d",time);
	strcat(strcat(strcat(strcpy(str,str_1),tempNum),str_2),temp);
	rc = mysql_real_query(&mysql, str, strlen(str));
        if (0 != rc) {
                printf("mysql_real_query(): %s\n", mysql_error(&mysql));
                return -1;
        }
	mysql_close(&mysql);
	return 0;
}

void user_function(int userNum,int reFlag) {
	time_t timep;
        struct tm *p;
        int  flag;
	int time_I,time_O;
        time(&timep);
        p = gmtime(&timep);
	system("clear");
	if(timeAuth(User[userNum].time,p->tm_min) == 0) {
		if (reFlag == 0) {
			printf("Take Your Umbrella!,%s\n",User[userNum].name);
                	User[userNum].returnFlag = "1";
			if(write_db_tm(userNum,p->tm_min) == 0)
				printf("Time has been recorded\n");
               		if(write_db_re(userNum,"1") == 0)  
				printf("State has been updated\n");
		}
		else {
			printf("%s:have a nice day!\n",User[userNum].name);
               	 	User[userNum].returnFlag = "0";
			if(write_db_tm(userNum,1000) ==0 )
				printf("Time record has been cleared\n");
                	if(write_db_re(userNum,"0") == 0) 
				printf("State has been updated\n");
		}
		delay(3000);
        	digitalWrite(userNum,HIGH);
        	delay(1000);
        	digitalWrite(userNum,LOW);
	}
	else {
		delay(1000);
		printf("Blocked user!\n");
	}
}

int main(){
	int fd;
	char data[] = {0x03,0x08,0xC1,0x20,0x02,0x00,0x00,0x17};
	int readData;
	int ID[ID_LENTH];
	int i;
	wiringPiSetup();
	pinMode(0,OUTPUT);
	pinMode(1,OUTPUT);
	pinMode(2,OUTPUT);
	pinMode(3,OUTPUT);
	pinMode(4,OUTPUT);
	pinMode(5,INPUT);
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
		if(digitalRead(5) == 1) {
			for(i = 0; i < ID_LENTH; i++) {
				readData = serialGetchar(fd);
				*(ID+i) = readData;
			}
			if(idAuth(ID) == -1){
				system("clear");
				printf("Permission denied!\n");
				digitalWrite(0,LOW);
				delay(4000);system("clear");
			}
			else{
				user_function(idAuth(ID),strcmp(User[idAuth(ID)].returnFlag,"0"));
			}
		}	
	}
	serialClose(fd);
	return 0;
}

