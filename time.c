#include<time.h>
#include<stdio.h>
int timeAuth(int time_O,int time_I) {
	if( (time_I - time_O < 0 && time_I > 2) || time_I - time_O > 2)
		return -1;
	else 
		return 0;
}

int main() {
	time_t timep;
	struct tm *p;
	int  flag;
	time(&timep);
	p = gmtime(&timep);
	flag = timeAuth(2,25);
	if(flag ==  0) printf("OK!\n");
	else printf("Error!\n");
	printf("%d\n",p->tm_year);
	printf("%d\n",p->tm_mon);
	printf("%d\n",p->tm_yday);
	printf("%s\n",ctime(&timep));
	
	return 0;
}
