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
    // remplissage des informations de serveur avec les infos passé en paramètre
	struct sockaddr_in Ad_dest;
	Ad_dest.sin_family = AF_INET;
	Ad_dest.sin_port = htons(atoi(argv[2])); //port du serveur qu'on passe en argument
	struct hostent* hp;
	hp = gethostbyname(argv[1]); // on obtient l'adresse IP via le nom de la machine
	if(hp == NULL){return -1;}
	memcpy(&Ad_dest.sin_addr,hp->h_addr_list[0],hp->h_length); // copie de l'adresse IP dans la struct 
    // partie connect pour le client
	int connect_client = connect(S1,(struct sockaddr *) &Ad_dest,sizeof(Ad_dest));
    // requete à envoyer
    char requete[100] = "GET /";
    char *file_name = argv[3]; // nom du fichier demandé
    char end_requete[100] = " HTTP/1.0\r\n\r\n";
    strcat(requete,file_name);
    strcat(requete,end_requete);
    char reponse[1200];
	printf("%s",requete);
    write(S1,requete,sizeof(requete));
    // lecture de ce que l'on reçoit
    read(S1,reponse,sizeof(reponse));
    printf("%s",reponse);
    // fermeture de la socket
    close(S1);
	return 0;
}