
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h> //pour avoir des informations supplémentaire sur les erreurs
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h> // module pour obtenir l'heure
#include <sys/select.h>

extern int errno;

int main(int argc, char* argv[])
{
	
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
	if(ecoute == -1){printf("erreur ecoute"); exit(EXIT_FAILURE);}

	int Ad1_size=sizeof(Ad1);
	char msg_recu[1000];
	
	
	char msg_envoie[50] = "message recu";
	//struct sockaddr_in Ad_emet;
	//int taille = sizeof(Ad_emet);
	//recvfrom(S1,msg_recu,sizeof(msg_recu),0,(struct sockaddr*)&Ad_emet,&taille);
	FILE * fp;
	char * line = NULL;
	char filename[100];
	size_t len = 0;
	ssize_t read_text;
	char text[1000];

	fd_set ens1; // création de l'ensemble des sockets 
	FD_ZERO(&ens1); // on "réinitialise l'ensemble" 
	//FD_SET(S1,&ens1); // on ajoute la socket créée dans l'ensemble

	int max_socket_value = S1; // on note la valeur de la plus grande socket 

	// boucle pour "écouter" les sockets
	while(1){
		
		// contient le nombre de socket 'acttives'
		FD_SET(S1,&ens1); // on ajoute la socket créée dans l'ensemble
		printf("while la valeur max est %d\n",max_socket_value);
		int select_statut = select(max_socket_value+1,&ens1,NULL,NULL,0);
		if (select_statut==-1){
			printf("Pb select ");
			exit(EXIT_FAILURE);
		}
		printf("On est après le select, %d\n",select_statut);
		
		// boucle for pour voir "s'il y a du nouveau"
		for (int i =0; i<= max_socket_value && select_statut >0;i++){
			// s'il y a qqch au niveau de la socket i 
			if (FD_ISSET(i,&ens1)){
				printf("On est dans le cas où i = S1\n");
				select_statut--;
				if (i==S1){
					// cas où la socket i est la socket S1 on accepte la connection
					int new = accept(S1,NULL,NULL);
					// cas où il y a une erreur lors du accepte
					if (new==-1){
						printf("Erreur dans l'accepte \n");
						exit(EXIT_FAILURE); 
						break;
					}
					FD_SET(new,&ens1);
					FD_CLR(i,&ens1);
					// maj de la valeur max de descripteur de socket
					if (max_socket_value < new){
						max_socket_value = new;
					}
				}

				// cas où la socket n'est pas la socket s1
				else {
					// on lit la requête que la socket à reçu
					int result = read(i,msg_recu,sizeof(msg_recu));
					if (result == -1){
						if (EWOULDBLOCK != errno){
							exit(EXIT_FAILURE);
						}
						break;
					}
					// on essaye de voir s'il y a un des fichiers présents dans le serveur sinon on lit le ficiher indiquant que la page est introuvable
					if(strstr(msg_recu,"index.html") != NULL)
					{
						fp = fopen("index.html","r");
						strcpy(filename,"index.html");// on enregistre le nom du fichier demandé
						if(fp == NULL){exit(EXIT_FAILURE);}
						while ((read_text = getline(&line, &len, fp)) != -1) 
						{
								printf("Retrieved line of length %zu:\n", read_text);
								strcat(text,line);
						}
					}
					else if(strstr(msg_recu,"verite.html") != NULL)
					{
						printf("verite.html");
						fp = fopen("verite.html","r");
						strcpy(filename,"verite.html");// on enregistre le nom du fichier demandé
						if(fp == NULL){exit(EXIT_FAILURE);}
						while ((read_text = getline(&line, &len, fp)) != -1) 
						{
								printf("Retrieved line of length %zu:\n", read_text);
								strcat(text,line);
						}
					}
					else if(strstr(msg_recu,"/ ") != NULL)
					{
						printf("/index.html");
						
						fp = fopen("index.html","r");
						strcpy(filename,"index.html"); // on enregistre le nom du fichier demandé
						if(fp == NULL){exit(EXIT_FAILURE);}
						while ((read_text = getline(&line, &len, fp)) != -1) 
						{
								printf("Retrieved line of length %zu:\n", read_text);
								strcat(text,line);
						}
					}
					else
					{
						printf("404.html");
						fp = fopen("404_error.html","r");
						strcpy(filename,"404_error.html");// on enregistre le nom du fichier demandé
						if(fp == NULL){exit(EXIT_FAILURE);}
						while ((read_text = getline(&line, &len, fp)) != -1) 
						{
								printf("Retrieved line of length %zu:\n", read_text);
								strcat(text,line);
						}
					}
					// on obtient l'heure à laquelle la requête a été faite
					time_t current_time;
					struct tm * time_info;
					time(&current_time);
					char time[10];
					time_info = localtime(&current_time);
					strftime(time,sizeof(time),"%H:%M:%S",time_info);

					// on essaye de voit qui a fait la requête 
					// on envoie une réponse pour dire que la requête à été accepté avec le type de fichier à prendre en compte
					write(i,"HTTP/1.1 200 OK Content-Type : text/html\r\n\r\n",strlen("HTTP/1.1 200 OK Content-Type : text/html\r\n\r\n"));
					write(i,text,sizeof(text)); // envoie du fichier HTML
					printf("The file requested was %s\n",filename);
					printf("At %s\n",time);
					printf("By %s\n");
					printf("%s\n",text);
					// réinitialisation des char 
					memset(text,0,sizeof(msg_recu));
					memset(msg_recu,0,sizeof(msg_recu));
					// fermeture des sockets + nettoyage de l'ensemble
					close(i);
					FD_CLR(i,&ens1);
					//max_socket_value = S1;
				}
			}		
		}
	}
	
	close(S1);
	FD_CLR(S1,&ens1);

	return 0;
}
