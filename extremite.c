#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLIGNE 64
#define PORT 123

int ext-in()
{
  char * hote; /* nom d'hÃ´te du  serveur */   
  char * port; /* port TCP du serveur */   
  char ip[NI_MAXHOST]; /* adresse IPv4 en notation pointÃ©e */
  struct addrinfo *resol; /* struct pour la rÃ©solution de nom */
  int s; /* descripteur de socket */

  /* Traitement des arguments */
  if (argc!=3) {/* erreur de syntaxe */
    printf("Usage: %s hote port\n",argv[0]);
    exit(1);
  }
  hote=argv[1]; /* nom d'hÃ´te du  serveur */   
  port=argv[2]; /* port TCP du serveur */   

  /* RÃ©solution de l'hÃ´te */
  if ( getaddrinfo(hote,port,NULL, &resol) < 0 ){
    perror("rÃ©solution adresse");
    exit(2);
  }

  /* On extrait l'addresse IP */
  sprintf(ip,"%s",inet_ntoa(((struct sockaddr_in*)resol->ai_addr)->sin_addr));

  /* CrÃ©ation de la socket, de type TCP / IP */
  /* On ne considÃ¨re que la premiÃ¨re adresse renvoyÃ©e par getaddrinfo */
  if ((s=socket(resol->ai_family,resol->ai_socktype, resol->ai_protocol))<0) {
    perror("allocation de socket");
    exit(3);
  }
  fprintf(stderr,"le nÂ° de la socket est : %i\n",s);

  /* Connexion */
  fprintf(stderr,"Essai de connexion Ã  %s (%s) sur le port %s\n\n",
	  hote,ip,port);
  if (connect(s,resol->ai_addr,sizeof(struct sockaddr_in))<0) {
    perror("connexion");
    exit(4);
  }
  freeaddrinfo(resol); /* /!\ LibÃ©ration mÃ©:wqmoire */

  /* Session */
  char tampon[MAXLIGNE + 3]; /* tampons pour les communications */
  ssize_t lu;
  int fini=0;
  while( 1 ) { 
    /* Jusqu'Ã  fermeture de la socket (ou de stdin)     */
    /* recopier Ã  l'Ã©cran ce qui est lu dans la socket  */
    /* recopier dans la socket ce qui est lu dans stdin */

    /* rÃ©ception des donnÃ©es */
    lu = recv(s,tampon,MAXLIGNE,0); /* bloquant */
    if (lu == 0 ) {
      fprintf(stderr,"Connexion terminÃ©e par l'hÃ´te distant\n");
      break; /* On sort de la boucle infinie */
    }
    tampon[lu] = '\0';
    printf("reÃ§u: %s",tampon);
    if ( fini == 1 )
      break;  /* on sort de la boucle infinie*/
    
    /* recopier dans la socket ce qui est entrÃ© au clavier */    
    if ( fgets(tampon,MAXLIGNE - 2,stdin) == NULL ){/* entrÃ©e standard fermÃ©e */
      fini=1;
      fprintf(stderr,"Connexion terminÃ©e !!\n");
      fprintf(stderr,"HÃ´te distant informÃ©...\n");
      shutdown(s, SHUT_WR); /* terminaison explicite de la socket 
			     dans le sens client -> serveur */
      /* On ne sort pas de la boucle tout de suite ... */
    }else{   /* envoi des donnÃ©es */
      send(s,tampon,strlen(tampon),0);
    }
  } 
  /* Destruction de la socket */
  close(s);

  fprintf(stderr,"Fin de la session.\n");
  return EXIT_SUCCESS;
}


void echo(int f, char* hote, char* port);

int ext-out()
{
  int s,n; /* descripteurs de socket */
  int len,on; /* utilitaires divers */
  struct addrinfo * resol; /* rÃ©solution */
  struct addrinfo indic = {AI_PASSIVE, /* Toute interface */
                           PF_INET,SOCK_STREAM,0, /* IP mode connectÃ© */
                           0,NULL,NULL,NULL};
  struct sockaddr_in client; /* adresse de socket du client */
  char * port; /* Port pour le service */
  int err; /* code d'erreur */
  
  /* Traitement des arguments */
  if (argc!=2) { /* erreur de syntaxe */
    printf("Usage: %s port\n",argv[0]);
    exit(1);
  }
  
  port=argv[1]; fprintf(stderr,"Ecoute sur le port %s\n",port);
  err = getaddrinfo(NULL,port,&indic,&resol); 
  if (err<0){
    fprintf(stderr,"RÃ©solution: %s\n",gai_strerror(err));
    exit(2);
  }

  /* CrÃ©ation de la socket, de type TCP / IP */
  if ((s=socket(resol->ai_family,resol->ai_socktype,resol->ai_protocol))<0) {
    perror("allocation de socket");
    exit(3);
  }
  fprintf(stderr,"le nÂ° de la socket est : %i\n",s);

  /* On rend le port rÃ©utilisable rapidement /!\ */
  on = 1;
  if (setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0) {
    perror("option socket");
    exit(4);
  }
  fprintf(stderr,"Option(s) OK!\n");

  /* Association de la socket s Ã  l'adresse obtenue par rÃ©solution */
  if (bind(s,resol->ai_addr,sizeof(struct sockaddr_in))<0) {
    perror("bind");
    exit(5);
  }
  freeaddrinfo(resol); /* /!\ LibÃ©ration mÃ©moire */
  fprintf(stderr,"bind!\n");

  /* la socket est prÃªte Ã  recevoir */
  if (listen(s,SOMAXCONN)<0) {
    perror("listen");
    exit(6);
  }
  fprintf(stderr,"listen!\n");

  while(1) {
    /* attendre et gÃ©rer indÃ©finiment les connexions entrantes */
    len=sizeof(struct sockaddr_in);
    if( (n=accept(s,(struct sockaddr *)&client,(socklen_t*)&len)) < 0 ) {
      perror("accept");
      exit(7);
    }
    /* Nom rÃ©seau du client */
    char hotec[NI_MAXHOST];  char portc[NI_MAXSERV];
    err = getnameinfo((struct sockaddr*)&client,len,hotec,NI_MAXHOST,portc,NI_MAXSERV,0);
    if (err < 0 ){
      fprintf(stderr,"rÃ©solution client (%i): %s\n",n,gai_strerror(err));
    }else{ 
      fprintf(stderr,"accept! (%i) ip=%s port=%s\n",n,hotec,portc);
    }
    /* traitement */
    echo(n,hotec,portc);
  }
  return EXIT_SUCCESS;
}

/* echo des messages reÃ§us (le tout via le descripteur f) */
void echo(int f, char* hote, char* port)
{
  ssize_t lu; /* nb d'octets reÃ§us */
  char msg[MAXLIGNE+1]; /* tampons pour les communications */ 
  char tampon[MAXLIGNE+1]; 
  int pid = getpid(); /* pid du processus */
  int compteur=0;
  
  /* message d'accueil */
  snprintf(msg,MAXLIGNE,"Bonjour %s! (vous utilisez le port %s)\n",hote,port);
  /* envoi du message d'accueil */
  send(f,msg,strlen(msg),0);
  
  do { /* Faire echo et logguer */
    lu = recv(f,tampon,MAXLIGNE,0);
    if (lu > 0 )
      {
        compteur++;
        tampon[lu] = '\0';
        /* log */
        fprintf(stderr,"[%s:%s](%i): %3i :%s",hote,port,pid,compteur,tampon);
        snprintf(msg,MAXLIGNE,"> %s",tampon);
        /* echo vers le client */
        send(f, msg, strlen(msg),0);
      } else {
        break;
      }
  } while ( 1 );
       
  /* le correspondant a quittÃ© */
  send(f,CIAO,strlen(CIAO),0);
  close(f);
  fprintf(stderr,"[%s:%s](%i): TerminÃ©.\n",hote,port,pid);
}

