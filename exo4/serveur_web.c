
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h> //pour avoir des informations supplémentaire sur les erreurs
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

extern int errno;

int main(int argc, char* argv[])
{
	int pid = getpid();
	printf("PID : %d \n",pid);
	int S1 = socket(AF_INET, SOCK_STREAM,0);
	struct sockaddr_in Ad1;
	Ad1.sin_family = AF_INET;
	Ad1.sin_port = htons(atoi(argv[1])); //port du client
	Ad1.sin_addr.s_addr = INADDR_ANY;
	memset(Ad1.sin_zero,0,8);
	int res = bind(S1,(struct sockaddr*)&Ad1,sizeof(Ad1));
	if(res==-1)
	{
		if(errno==EBADF){printf("Pb bind : mauvais descripteur");}
		if(errno==EINVAL){printf("Socket deja lie à une adresse");}
	}
	
	int ecoute = listen(S1,5);
	if(ecoute == -1){printf("erreur ecoute");}

	int Ad1_size=sizeof(Ad1);
	printf("On est avant l'accepte \n");
	int service = accept(S1,(struct sockaddr*)&Ad1,&Ad1_size);
	printf("On est après l'accepte \n");
	char msg_recu[1000];
	read(service,msg_recu,sizeof(msg_recu));
	int pid_recu;
	char msg_envoie[50] = "message recu";
	//struct sockaddr_in Ad_emet;
	//int taille = sizeof(Ad_emet);
	//recvfrom(S1,msg_recu,sizeof(msg_recu),0,(struct sockaddr*)&Ad_emet,&taille);
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char text[1000];
	if(strstr(msg_recu,"index.html") != NULL)
	{
		fp = fopen("index.html","r");
		if(fp == NULL){exit(EXIT_FAILURE);}
		while ((read = getline(&line, &len, fp)) != -1) 
		{
        		printf("Retrieved line of length %zu:\n", read);
        		strcat(text,line);
    		}
	}
	else if(strstr(msg_recu,"verite.html") != NULL)
	{
		printf("verite.html");
		fp = fopen("verite.html","r");
		if(fp == NULL){exit(EXIT_FAILURE);}
		while ((read = getline(&line, &len, fp)) != -1) 
		{
        		printf("Retrieved line of length %zu:\n", read);
        		strcat(text,line);
    		}
	}
	else if(strstr(msg_recu,"/ ") != NULL)
	{
		printf("/index.html");
		
		fp = fopen("index.html","r");
		if(fp == NULL){exit(EXIT_FAILURE);}
		while ((read = getline(&line, &len, fp)) != -1) 
		{
        		printf("Retrieved line of length %zu:\n", read);
        		strcat(text,line);
    		}
	}
	else
	{
		printf("404.html");
		fp = fopen("404_error.html","r");
		if(fp == NULL){exit(EXIT_FAILURE);}
		while ((read = getline(&line, &len, fp)) != -1) 
		{
        		printf("Retrieved line of length %zu:\n", read);
        		strcat(text,line);
    		}
	}
	printf("%s \n",msg_recu);
	printf("%s \n",text);
	write(service,text,sizeof(text));
	close(service);
	close(S1);

	return 0;
}
