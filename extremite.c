#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAXLIGNE 64
#define PORT "123"

typedef struct{
	  int fd;
    int socket;
} ThreadData;

void * out_handler(void *args){
  printf("[OUT HANDLER] 0\n");
  ssize_t lu; /* nb d'octets reÃ§us */
  char msg[MAXLIGNE+1]; /* tampons pour les communications */ 
  char tampon[MAXLIGNE+1]; 
  int compteur=0;
  
  ThreadData *threadData = args;

	int socket = threadData->socket;
  int fd = threadData->fd;

  printf("[OUT HANDLER] 1 | %d %d\n", socket, fd);
  
  do { /* Faire echo et logguer */
    printf("[OUT HANDLER] 2\n");
    lu = recv(socket,tampon,MAXLIGNE,0);
    printf("[OUT HANDLER] 2.5 | %ld\n", lu);
    if (lu > 0 )
      {
        printf("[OUT HANDLER] 3\n");
        compteur++;
        tampon[lu] = '\0';
        /* log */
        //fprintf(stderr,"[%s:%s](%i): %3i :%s",hote,port,pid,compteur,tampon);
        snprintf(msg,MAXLIGNE,"%s",tampon);
        printf("%s",msg);

        size_t nbytes;
        nbytes = sizeof(tampon);

        write(fd, tampon, nbytes);
        /* echo vers le client */
        //send(f, msg, strlen(msg),0);
        printf("[OUT HANDLER] 4\n");
      } else {
        break;
      }
  } while ( 1 );
       
  printf("[OUT HANDLER] 5\n");
  /* le correspondant a quittÃ© */
  close(socket);
  //fprintf(stderr,"[%s:%s](%i): Terminé.\n",hote,port,pid);

  printf("[OUT HANDLER] EXIT\n");
  pthread_exit(NULL);
}

void * in_handler(void *args){
  printf("[IN HANDLER] 0\n");
  char tampon[MAXLIGNE + 3]; /* tampons pour les communications */
  char msg[MAXLIGNE+1]; /* tampons pour les communications */ 

  int fini=0;

  ThreadData *threadData = args;

	int socket = threadData->socket;
  int fd = threadData->fd;

  

  printf("[IN HANDLER] 1 | %d %d\n", socket, fd);
  while( 1 ) {     
    printf("[IN HANDLER] 2\n");
    if ( fini == 1 ) break;

    size_t nbytes;
    ssize_t bytes_read;
    nbytes = sizeof(tampon);
    bytes_read = read(fd, tampon, nbytes);
    printf("[IN HANDLER] 3\n");
    if (bytes_read == 0 ){/* entrÃ©e standard fermÃ©e */
      printf("[IN HANDLER] 4\n");
      fini=1;
      shutdown(socket, SHUT_WR); 
    }else{  
      printf("[IN HANDLER] 5 | %ld\n", bytes_read);
      snprintf(msg,MAXLIGNE,"%s",tampon);
      printf("%s",msg);
      send(socket,tampon,strlen(tampon),0);
    }
  } 
  /* Destruction de la socket */
  close(socket);

  printf("[IN HANDLER] 6\n");
  fprintf(stderr,"Fin de la session.\n");

  printf("[IN HANDLER] EXIT\n");
  pthread_exit(NULL);
}




int ext_in(char * hote, int fd, char * port)
{  
  //char ip[NI_MAXHOST]; /* adresse IPv4 en notation pointÃ©e */
  //struct addrinfo *resol; /* struct pour la rÃ©solution de nom */
  struct sockaddr_in6 server_addr;
  int s; /* descripteur de socket */

  /* RÃ©solution de l'hÃ´te */
  /*
  if ( getaddrinfo(hote,port,NULL, &resol) < 0 ){
    perror("résolution adresse");
    exit(2);
  }*/

  /* On extrait l'addresse IP */
  /*
  sprintf(ip,"%s",inet_ntoa(((struct sockaddr_in*)resol->ai_addr)->sin_addr));
  */
  /* CrÃ©ation de la socket, de type TCP / IP */
  /* On ne considÃ¨re que la premiÃ¨re adresse renvoyÃ©e par getaddrinfo */

  if ((s=socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP))<0) {
    perror("allocation de socket");
    exit(3);
  }
  fprintf(stderr,"le n° de la socket est : %i\n",s);

  //bzero((char *) &server_addr, sizeof(server_addr));

  server_addr.sin6_family = AF_INET6;
  server_addr.sin6_port = htons(atoi(port));
  inet_pton(AF_INET6, hote, &server_addr.sin6_addr);

  /* Connexion */
  fprintf(stderr, "Essai de connexion à %s sur le port %s\n\n", hote, port);
  if (connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("connexion");
    exit(4);
  }
  //freeaddrinfo(resol); /* /!\ LibÃ©ration mÃ©:wqmoire */


  /* Session */
  pthread_t thread_in;
  pthread_t thread_out;
  ThreadData *args = malloc(sizeof *args);
  args->socket = s;
  args->fd = fd;

  pthread_create(&thread_out, NULL, out_handler, args);
  pthread_create(&thread_in, NULL, in_handler, args);


  pthread_join(thread_out, NULL);
  pthread_join(thread_in, NULL);

  fprintf(stderr,"Fin de la session.\n");
  return EXIT_SUCCESS;
}


int ext_out(int fd, char * port)
{
  int s,n; /* descripteurs de socket */
  int on; /* utilitaires divers */
  //struct addrinfo * resol; /* rÃ©solution */
  //struct addrinfo indic = {AI_PASSIVE, /* Toute interface */
  //                         PF_INET,SOCK_STREAM,0, /* IP mode connectÃ© */
  //                         0,NULL,NULL,NULL};
  //struct sockaddr_in client; /* adresse de socket du client */
  //int err; /* code d'erreur */

  socklen_t client_addr_len;
  struct sockaddr_in6 addr, client_addr;
  addr.sin6_family = AF_INET6;
  addr.sin6_port = htons(atoi(port));
  addr.sin6_addr = in6addr_any;

  /*
  err = getaddrinfo(NULL,port,&indic,&resol); 
  if (err<0){
    fprintf(stderr,"Résolution: %s\n",gai_strerror(err));
    exit(2);
  }
  */
  /* CrÃ©ation de la socket, de type TCP / IP */
  if ((s=socket(AF_INET6, SOCK_STREAM, 0))<0) {
    perror("allocation de socket");
    exit(3);
  }

  /* On rend le port rÃ©utilisable rapidement /!\ */
  on = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
    perror("option socket");
    exit(4);
  }

  /* Association de la socket s Ã  l'adresse obtenue par rÃ©solution */
  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(5);
  }
  //freeaddrinfo(resol); /* /!\ LibÃ©ration mÃ©moire */

  /* la socket est prÃªte Ã  recevoir */
  if (listen(s,SOMAXCONN)<0) {
    perror("listen");
    exit(6);
  }


    /* attendre et gÃ©rer indÃ©finiment les connexions entrantes */
    //len=sizeof(struct sockaddr_in);
    client_addr_len = sizeof(client_addr);
    if( (n=accept(s,(struct sockaddr *)&client_addr, &client_addr_len)) < 0 ) {
      perror("accept");
      exit(7);
    }


    /* Nom rÃ©seau du client */
    /*
    char hotec[NI_MAXHOST];  char portc[NI_MAXSERV];
    err = getnameinfo((struct sockaddr*)&client,len,hotec,NI_MAXHOST,portc,NI_MAXSERV,0);
    if (err < 0 ){
      fprintf(stderr,"résolution client (%i): %s\n",n,gai_strerror(err));
    }else{ 
      fprintf(stderr,"accept! (%i) ip=%s port=%s\n",n,hotec,portc);
    }
    */

    /* traitement */
    pthread_t thread_in;
    pthread_t thread_out;
		ThreadData *args = malloc(sizeof *args);
		args->socket = n;
		args->fd = fd;

    ThreadData *args2 = malloc(sizeof *args2);
		args2->socket = n;
		args2->fd = fd;

		pthread_create(&thread_out, NULL, out_handler, args);
    pthread_create(&thread_in, NULL, in_handler, args2);
   


    pthread_join(thread_out, NULL);
    pthread_join(thread_in, NULL);

  return EXIT_SUCCESS;
}