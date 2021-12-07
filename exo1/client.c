#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h> //pour avoir des informations supplémentaire sur les erreurs
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

extern int errno;

int main(int argc, char* argv[])
{
	char msg_recu[50];
	char msg_envoie[50];
	strcpy(msg_envoie, argv[3]);
	// création de la socket 
	int S1 = socket(AF_INET, SOCK_DGRAM,0);

	// remplissage des champs pour utilier la socket 
	struct sockaddr_in Ad1;
	Ad1.sin_family = AF_INET;
	Ad1.sin_port = htons(10001); //port du client
	Ad1.sin_addr.s_addr = INADDR_ANY;
	memset(Ad1.sin_zero,0,8);
	int res = bind(S1,(struct sockaddr*)&Ad1,sizeof(Ad1));
	if(res==-1)
	{
		if(errno==EBADF){printf("Pb bind : mauvais descripteur");}
		if(errno==EINVAL){printf("Socket deja lie à une adresse");}
	}

	int pid = getpid();
	int pid_serveur;
	printf("PID : %d \n", pid);
	printf("chaine de chaine de caracteres : %s \n", msg_envoie);
	
	// remplissage des informations de serveur avec les infos passé en paramètre
	struct sockaddr_in Ad_dest;
	Ad_dest.sin_family = AF_INET;
	Ad_dest.sin_port = htons(atoi(argv[2])); //port du serveur qu'on passe en argument
	struct hostent* hp;
	hp = gethostbyname(argv[1]);
	if(hp == NULL){return -1;}
	memcpy(&Ad_dest.sin_addr,hp->h_addr_list[0],hp->h_length);

	// envoie des messages au serveur
	sendto(S1,msg_envoie,sizeof(msg_envoie),0,(struct sockaddr*)&Ad_dest,sizeof(Ad_dest));
	sendto(S1,&pid,sizeof(pid),0,(struct sockaddr*)&Ad_dest,sizeof(Ad_dest));
	printf("Les messages viennent d'être envoyé\n");
	// lecture de la réponse du serveur 
	int taille = sizeof(Ad_dest);
	recvfrom(S1,msg_recu,sizeof(msg_recu),0,(struct sockaddr*)&Ad_dest,&taille);
	printf("%s \n",msg_recu);

	recvfrom(S1,&pid_serveur,sizeof(pid_serveur),0,(struct sockaddr*)&Ad_dest,&taille);
	printf("%d \n",pid_serveur);
	return 0;
}
