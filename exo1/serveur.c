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
	int pid = getpid(); // obtention du pid 
	printf("PID : %d \n",pid); // affichage du pid 
	int S1 = socket(AF_INET, SOCK_DGRAM,0); // création de la socket 
	// remplissage des différents champs pour 
	struct sockaddr_in Ad1;
	Ad1.sin_family = AF_INET;
	Ad1.sin_port = htons(atoi(argv[1])); //port du serveur
	Ad1.sin_addr.s_addr = INADDR_ANY; // prend l'adresse IP de la machine 
	memset(Ad1.sin_zero,0,8);
	int res = bind(S1,(struct sockaddr*)&Ad1,sizeof(Ad1));
	if(res==-1)
	{
		if(errno==EBADF){printf("Pb bind : mauvais descripteur");}
		if(errno==EINVAL){printf("Socket deja lie à une adresse");}
	}
	char msg_recu[50];
	int pid_recu;
	char msg_envoie[50] = "message recu";
	struct sockaddr_in Ad_emet;
	int taille = sizeof(Ad_emet);
	// lecture + affichage du message se trouvant dans le buffer 
	recvfrom(S1,msg_recu,sizeof(msg_recu),0,(struct sockaddr*)&Ad_emet,&taille);
	printf("%s \n",msg_recu);
	// lecture + affichage du numéro de PID du client se trouvant dans le buffer 
	recvfrom(S1,&pid_recu,sizeof(pid_recu),0,(struct sockaddr*)&Ad_emet,&taille);
	printf("%d \n", pid_recu);
	//envoie du message reçue par le serveur avec son pid à lui
	sendto(S1,msg_recu,sizeof(msg_recu),0,(struct sockaddr*)&Ad_emet,sizeof(Ad_emet));
	sendto(S1,&pid,sizeof(pid),0,(struct sockaddr*)&Ad_emet,sizeof(Ad_emet));
	printf("Les messages viennent d'être renvoyé au client \n");
	return 0;	
}
