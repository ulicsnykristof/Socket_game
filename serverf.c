#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

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
	}else if(strcmp(tipus, "bind")==0){
		perror("Bind hiba\n");
		exit(1);
	}else{
		perror("Listen hiba\n");
		exit(1);
	}
}

void kiir(int i, int tomb_hossz, int tomb[]){
	int k;
	printf("*************************\n");
	printf("%d. játékos tétje:\n", i);
	printf("------------------\n");
	for(k=0;k<tomb_hossz;k++){
		printf("%d. tétje a %d számon\n", k+1, tomb[k]);
	}
	printf("*************************\n\n");

}

int main(int argc, char *argv[]){
	
/***************************************************************************/
/***VARIABLES***************************************************************/	
	
	int i;
	int kezdo_coin, korok_szama;
	int coin[2];
	int fd, fdc[2];
	int ip;
	char *ipstr;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int server_size = sizeof server;
	int client_size = sizeof client;
	char sendBuff[1025];
	char receBuff[1025];
	int bytes = 1024;
	int flags = 0;
	int on = 1;
	int halt[2];
	int vege = 0;
	
/***************************************************************************/
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT_NO);
	
	kezdo_coin = atoi(argv[2]);
	korok_szama = atoi(argv[1]);
	coin[0] = kezdo_coin;
	coin[1] = kezdo_coin;
	halt[0] = 0;
	halt[1] = 0;

	srand(time(NULL));
	
/***************************************************************************/
	
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) hiba("socket");

	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof on);
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof on);
	
	if(bind(fd, (struct sockaddr *) &server, server_size) == -1) hiba("bind");
	
	printf("Rulett játék\n");
	printf("Játékosok csatlakozására vár...\n\n");
	
/***************************************************************************/
	
	i = 0;
	while(i < 2){
		if(listen(fd, 2) == -1) hiba("listen");	
		if((fdc[i] = accept(fd, (struct sockaddr *) &client, &client_size)) == -1) hiba("accept");

		ip = client.sin_addr.s_addr;
    	ipstr = inet_ntoa(server.sin_addr);
	
		sprintf(sendBuff, "Köszöntelek a játékban!\n");
		if(send(fdc[i], sendBuff, bytes, flags) == -1) hiba("send");
		
		printf("%d. Játékos becsatlakozott.\n", i);
		if(i==0) printf("Várakozás a 2. játékosra...\n");
		i++;
	}
		
	i = 0;
	while(i<2){
		sprintf(sendBuff, "%d",	kezdo_coin);
		if(send(fdc[i], sendBuff, bytes, flags) == -1) hiba("send");	
		i++;
	}
	printf("\n");
	
/***************************************************************************/
/***************BEGIN*******************************************************/
	
	int n = 0;
	while(1){
		n++;
		printf("%d. kör, a játékosok tegyék meg tétjeiket\n", n);
		
/***************DONE********************************************************/
		
		i=0;
		int befejez[2];
		while(i<2){
			if(recv(fdc[i], receBuff, bytes, flags) == -1) hiba("recv");
			befejez[i] = atoi(receBuff);
			i++;
		}
		
		sprintf(sendBuff, "%d", befejez[1]);
		if(send(fdc[0], sendBuff, bytes, flags) == -1) hiba("send");			
		sprintf(sendBuff, "%d",	befejez[0]);
		if(send(fdc[1], sendBuff, bytes, flags) == -1) hiba("send");
		
		if(befejez[0]==1 && befejez[1]==1){
			printf("Mindkét játékos befejzezte a játékot!\n");
			break;
		}else if(befejez[0]==1){
			printf("Az első játékos befejezte a játékot\n");
			break;
		}else if(befejez[1]==1){
			printf("A második játékos befejezte a játékot\n");
			break;
		}
		
/******PRE-GAME****************************************************************/		
				
		int tetek_szama[2];
		i=0;
		while(i<2){
			if(recv(fdc[i], receBuff, bytes, flags) == -1) hiba("recv");
			tetek_szama[i] = atoi(receBuff);
			i++;
		}
		
		int tetek_helye_elso[3];
		int tetek_helye_masodik[3];
		
		i=0;
		while(i<2){
			int k;
			for(k=0;k<tetek_szama[i];k++){
				if(recv(fdc[i], receBuff, bytes, flags) == -1) hiba("recv");
				if(i==0) tetek_helye_elso[k] = atoi(receBuff);
				else tetek_helye_masodik[k] = atoi(receBuff);
			}
			i++;
		}

		kiir(1, tetek_szama[0], tetek_helye_elso);
		kiir(2, tetek_szama[1], tetek_helye_masodik);
		
/*****STAGE 1********************************************************************/
		
		int g=0;
		printf("Pörög a kerék");
		while(g<4){
			printf(".");
			sleep(1);
			g++;
		}
		printf("\n\n");
		
		int porgetett = rand()%10;
		coin[0] = coin[0] - tetek_szama[0];
		coin[1] = coin[1] - tetek_szama[1];
		
		int z;
		for(z=0;z<tetek_szama[0];z++){
			if(porgetett == tetek_helye_elso[z]){
				if(tetek_szama[0] == 1){
					coin[0] = coin[0]+6*tetek_szama[0];
				}else if(tetek_szama[0] == 2){
					coin[0] = coin[0]+3*tetek_szama[0];
				}else{
					coin[0] = coin[0]+2*tetek_szama[0];
				}
			}
		}
		
		for(z=0;z<tetek_szama[1];z++){
			if(porgetett == tetek_helye_masodik[z]){
				if(tetek_szama[1] == 1){
					coin[1] = coin[1]+6*tetek_szama[1];
				}else if(tetek_szama[1] == 2){
					coin[1] = coin[1]+3*tetek_szama[1];
				}else{
					coin[1] = coin[1]+2*tetek_szama[1];
				}
			}
		}
		
		printf("***************************\n\n");
		printf("A pörgetett szám a(z): %d\n\n", porgetett);
		printf("Játék állása:\n");
		printf("-------------\n\n");
		printf("Első játékos zsetonjai: %d\n", coin[0]);
		printf("Második játékos zsetonjai: %d\n", coin[1]);
		printf("\n***************************\n\n");
		
/*******STAGE 2******************************************************************/				
		
		i=0;
		while(i<2){
			sprintf(sendBuff, "%d",	coin[i]);
			if(send(fdc[i], sendBuff, bytes, flags) == -1) hiba("send");
			i++;
		}
		
		i=0;
		while(i<2){
			sprintf(sendBuff, "%d", porgetett);
			if(send(fdc[i], sendBuff, bytes, flags) == -1) hiba("send");
			i++;
		}			
					
		sprintf(sendBuff, "%d",	coin[1]);
		if(send(fdc[0], sendBuff, bytes, flags) == -1) hiba("send");			
		sprintf(sendBuff, "%d",	coin[0]);
		if(send(fdc[1], sendBuff, bytes, flags) == -1) hiba("send");			
					
/******STAGE 3*******************************************************************/						
										
		if(coin[0] <= 0) halt[0] = 1;	
		if(coin[1] <= 0) halt[1] = 1;
		if(halt[0]==1 || halt[1]==1) vege = 1;
		if(n == korok_szama) vege = 2;
		
		i=0;
		while(i<2){
			sprintf(sendBuff, "%d",	vege);
			if(send(fdc[i], sendBuff, bytes, flags) == -1) hiba("send");
			i++;
		}			
					
		sprintf(sendBuff, "%d",	halt[1]);
		if(send(fdc[0], sendBuff, bytes, flags) == -1) hiba("send");
		
		sprintf(sendBuff, "%d",	halt[0]);
		if(send(fdc[1], sendBuff, bytes, flags) == -1) hiba("send");			
		
		if(vege==1){
			printf("*******************************\n");
			printf("Játszma vége\n\n");
			printf("1. játékos zsetonjai = %d\n", coin[0]);
			printf("2. játékos zsetonjai = %d\n", coin[1]);
			if(coin[0]>coin[1]){
				printf("1. játékos nyert\n");
			}else if(coin[1]>coin[0]){
				printf("2. játékos nyert\n");
			}else{
				printf("Döntetlen\n");
			}
			printf("*******************************\n");
		}else if(vege==2){
			printf("*******************************\n");
			printf("Játszma vége, elfogytak a körök\n\n");
			printf("1. játékos zsetonjai = %d\n", coin[0]);
			printf("2. játékos zsetonjai = %d\n\n", coin[1]);
			if(coin[0]>coin[1]){
				printf("1. játékos nyert\n");
			}else if(coin[1]>coin[0]){
				printf("2. játékos nyert\n");
			}else{
				printf("Döntetlen\n");
			}
			printf("*******************************\n");
		}
		
/******STAGE 4*******************************************************************/
		
		int ujra[2];
		if(vege == 1 || vege == 2){
			i=0;
			while(i<2){
				if(recv(fdc[i], receBuff, bytes, flags) == -1) hiba("recv");
				ujra[i] = atoi(receBuff);
				i++;
			}
		
			sprintf(sendBuff, "%d",	ujra[1]);
			if(send(fdc[0], sendBuff, bytes, flags) == -1) hiba("send");
			sprintf(sendBuff, "%d",	ujra[0]);
			if(send(fdc[1], sendBuff, bytes, flags) == -1) hiba("send");
			
			if(ujra[0] == 0 || ujra[1] == 0){
				printf("Játékosok befejezték a játékot, jöjjenek máskor is!\n");
				break;
			}else{
				printf("Új játék indul\n");
				coin[0] = kezdo_coin;
				coin[1] = kezdo_coin;
				halt[0] = 0;
				halt[1] = 0;
				vege = 0;
				n = 0;
			}
		}
		
		
	}
	close(fd);
	close(fdc[0]);
	close(fdc[1]);
	return 0;
}
