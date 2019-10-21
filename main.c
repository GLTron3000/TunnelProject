#include <stdlib.h>
#include <stdio.h>
#include "extremite.h"
#include "iftun.h"

int main (int argc, char** argv){
  int tunfd;
  printf("Création de %s\n",argv[1]);
  tunfd = tun_alloc(argv[1]);
  tun_load_config(argv[1]);

  if(argc == 3){
    char * hote = argv[2];
    //tun_copy_output(tunfd, 1);
    ext_in(hote, tunfd);

    printf("Appuyez sur une touche pour terminer\n");
    getchar();
  }else{
    printf("Ecoute\n");

    ext_out(tunfd);

    printf("Appuyez sur une touche pour terminer\n");
    getchar();
  }

  

  return 0;
}