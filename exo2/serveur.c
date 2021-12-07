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

	int S1 = socket(AF_INET, SOCK_STREAM,0); // création de la socket 
	// remplissage des champs de la socket 
	struct sockaddr_in Ad1;
	Ad1.sin_family = AF_INET;
	Ad1.sin_port = htons(atoi(argv[1])); //port d'écoute
	Ad1.sin_addr.s_addr = INADDR_ANY;
	memset(Ad1.sin_zero,0,8);
	int res = bind(S1,(struct sockaddr*)&Ad1,sizeof(Ad1));
	if(res==-1)
	{
		if(errno==EBADF){printf("Pb bind : mauvais descripteur");}
		if(errno==EINVAL){printf("Socket deja lie à une adresse");}
	}
	
	// on écoute ce que se trouve au niveau de la socket s1
	int ecoute = listen(S1,5);
	if(ecoute == -1){printf("erreur ecoute");}

	int Ad1_size=sizeof(Ad1);
	// on accepte la connextion 
	int service = accept(S1,(struct sockaddr*)&Ad1,(socklen_t *)&Ad1_size);
	char msg_recu[1200];
	// lecture et affichage de la requête lue par le serveur
	read(service,msg_recu,sizeof(msg_recu));
	printf("%s \n",msg_recu);
	int close_service = close(service);
	int close_s1 = close(S1);

	return 0;
}
