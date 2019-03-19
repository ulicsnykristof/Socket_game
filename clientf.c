#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>

#define SERVER_ADDR "127.0.0.1"
#define PORT_NO 6066


void hiba(char *tipus){	
	if(strcmp(tipus, "send")==0){
		perror("Send hiba\n");
		exit(1);
	}else if(strcmp(tipus, "recv")==0){
		perror("Receive hiba\n");
		exit(1);
	}else if(strcmp(tipus, "accept")==0){
		perror("Accept hiba\n");
		exit(1);
	}else if(strcmp(tipus, "socket")==0){
		perror("Socket hiba\n");
		exit(1);
	}else if(strcmp(tipus, "connect")==0){
		perror("Connect hiba\n");
		exit(1);
	}else{
		perror("Listen hiba\n");
		exit(1);
	}
}

int ending(){
	printf("Szeretnél új játékot kezdeni?\n (ujra / vege)\n");
	while(1){
		char *t;
		scanf("%s", t);
		if(strcmp(t, "ujra") == 0){
			return 1;
		}else if(strcmp(t, "vege") == 0){
			return 0;
		}else{
			fprintf(stderr, "Rossz parancs\n (ujra / vege)\n");
		}
	}
}


int main(){
	
/********************************************************************************************/	
	
	int fd, fdc;
	int flags = 0;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int server_size = sizeof server;
	int client_size = sizeof client;	
	char sendBuff[1025];
	char receBuff[1025];
	int bytes = 1024;
	int ip;
	int on = 1;
	char *ipstr;
	int my_coin = 0;
	int tovabb;
	int kezdo_coin;
	
/********************************************************************************************/
	
	ip = inet_addr(SERVER_ADDR);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = ip;
	server.sin_port = htons(PORT_NO);
	
/********************************************************************************************/
	
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) hiba("socket");

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);
	
	if(connect(fd, (struct sockaddr *) &server, server_size) == -1) hiba("connect");
	
	//ipstr = inet_ntoa(server.sin_addr);
	//printf("Connected to Server with IP address %s\n", ipstr);
	
	if(recv(fd, receBuff, bytes, flags) == -1) hiba("rece");
	printf("%s\n", receBuff);
	
	if(recv(fd, receBuff, bytes, flags) == -1) hiba("rece");
	my_coin = atoi(receBuff);
	kezdo_coin = my_coin;
	
	printf("Kezdő zsetonok száma = %d\n", my_coin);
	
/*****************************************************************************************/	
/*******BEGIN****************************************************************************/
	
	while(1){
		printf("Hány tétet szeretnél rakni? (max 3)\n");
		printf("Ha be szeretnéd fejezni a játékot, írd be: befejezem\n");
		
		int befejezem=0;
		int tetek_szama;
		while(1){
			int isdigi = 1;
			char valasz[30];
			scanf("%s", valasz);
			int q;
			for(q=0;q<strlen(valasz);q++){
				if(!isdigit(valasz[q])) isdigi=0;
			}
			
			if(isdigi==1){
				tetek_szama=atoi(valasz);
				if(tetek_szama>3 || tetek_szama<=0){
					fprintf(stderr, "1, 2 vagy 3 zsetont tehetsz fel egyszerre!\n");
				}else if(tetek_szama>my_coin){
					fprintf(stderr, "Nincs elég zsetonod hozzá!\n");
				}else{
					break;
				}
			}else{
				if(strcmp(valasz, "befejezem") == 0){
					befejezem=1;
					break;
				}else{
					fprintf(stderr, "Rossz parancs\n");
				}
			}
		}
		printf("Másik játékos választ...\n");
		
/******DONE******************************************************************************/
		
		sprintf(sendBuff, "%d", befejezem);
		if(send(fd, sendBuff, bytes, flags) == -1) hiba("send");
		
		int ellen_befejez;
		if(recv(fd, receBuff, bytes, flags) == -1) hiba("rece");
		ellen_befejez = atoi(receBuff);
		
		if(befejezem==1 && ellen_befejez==1){
			printf("Te és ellenfeled is befejzezte a játékot!\n");
			break;
		}else if(befejezem==1){
			printf("Befejezted a játékot\n");
			break;
		}else if(ellen_befejez==1){
			printf("Ellenfél befejezte a játékot\n");
			break;
		}
		
/******PRE-GAME****************************************************************************/
		
		
		printf("Hova szeretnéd tenni a zsetonokat? (0-9)\n");
		int tetek_helye[3];
		int k;
		int hely;
		char val[30];
		for(k=0;k<tetek_szama;k++){
			while(1){
				printf("%d. zseton?\n", k+1);
				scanf("%s", val);
				int d;
				int isdigi=1;
				for(d=0;d<strlen(val);d++){
					if(!isdigit(val[d])) isdigi=0;
				}
				if(isdigi==1){
					hely=atoi(val);
					if(hely>9 || hely<0){
						fprintf(stderr, "0 és 9 közötti számot adj meg!\n");
					}else if(k==1 && hely==tetek_helye[0] || k==2 && hely==tetek_helye[1]){
						fprintf(stderr, "Itt már van zsetonod!\n");
					}else{
						tetek_helye[k] = hely;
						break;
					}
				}else{
					fprintf(stderr, "Számot adj meg légy oly kedves :)\n");
				}
			}
		}
		
	
		sprintf(sendBuff, "%d", tetek_szama);
		if(send(fd, sendBuff, bytes, flags) == -1) hiba("send");
		
		
		for(k=0;k<tetek_szama;k++){
			sprintf(sendBuff, "%d", tetek_helye[k]);
			if(send(fd, sendBuff, bytes, flags) == -1) hiba("send");
		}
		
		printf("Másik játékos tétet tesz...\n");
		
/*****STAGE 1********************************************************************************/	
		
		
		
/*****STAGE 2*******************************************************************************/			
		
		if(recv(fd, receBuff, bytes, flags) == -1) hiba("rece");
		
		printf("\n**********************************\n");
		printf("Játék állása:\n");
		printf("-------------\n\n");
		
		if(my_coin > atoi(receBuff)){
			printf("Nincs találat\n");
		}else{
			printf("Találat\n");
		}		
		my_coin = atoi(receBuff);
		
		if(recv(fd, receBuff, bytes, flags) == -1) hiba("rece");
		printf("Pörgetett szám: %d\n", atoi(receBuff));
		printf("Zsetonok száma: %d\n", my_coin);
		
		if(recv(fd, receBuff, bytes, flags) == -1) hiba("rece");
		int enemy_coin = atoi(receBuff);
		
		printf("Ellenfél zsetonjai: %d\n", enemy_coin);
		printf("\n**********************************\n\n");	
		
/******STAGE 3******************************************************************************/		
		
		int ujra, ellen_ujra;
		
		if(recv(fd, receBuff, bytes, flags) == -1) hiba("rece");
		int vege = atoi(receBuff);
		if(recv(fd, receBuff, bytes, flags) == -1) hiba("rece");
		int ellen_halt = atoi(receBuff);
		if(vege==1){
			if(ellen_halt==1 && my_coin > 0){
				printf("GYŐZTÉL!!\n");
			}else if(ellen_halt==1 && my_coin <= 0){
				printf("DÖNTETLEN!!\n");
			}else{
				printf("VESZTETTÉL!!\n");
			}
			printf("A játék véget ért\n");
			ujra = ending();
		}else if(vege==2){
			printf("Elfogytak a körök!\n");
			if(my_coin > enemy_coin){
				printf("Több zsetonod maradt, szóval győztél(?)\n");
			}else if(my_coin < enemy_coin){
				printf("Kevesebb zsetonod maradt, szóval vesztettél(?)\n");
			}else{
				printf("Ugyanannyi zsetonod maradt, szóval döntetlen(?)\n");
			}
			ujra = ending();
		}
		
/******STAGE 4******************************************************************************/
		
		if(vege == 1 || vege == 2){
			sprintf(sendBuff, "%d", ujra);
			if(send(fd, sendBuff, bytes, flags) == -1) hiba("send");
		
			if(recv(fd, receBuff, bytes, flags) == -1) hiba("rece");
			ellen_ujra = atoi(receBuff);
		
			if(ujra == 0){
				printf("Nem szeretnél új játékot kezdeni\n");
				printf("Köszönjük hogy minket választott\n");
				break;
			}else if(ellen_ujra == 0){
				printf("Ellenfeled nem szeretne új játékot kezdeni\n");
				printf("Köszönjük hogy minket választott\n");
				break;
			}else{
				printf("Új játék indítása\n");
				my_coin = kezdo_coin;
				printf("Kezdő zsetonok száma = %d\n", my_coin);
			}
		}
	}
	close(fd);
	close(fdc);
	return 0;
}
