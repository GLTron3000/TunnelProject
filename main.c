#include <stdlib.h>
#include <stdio.h>
#include "extremite.h"
#include "iftun.h"

int main (int argc, char** argv){
  int tunfd;
  printf("CrÃ©ation de %s\n",argv[1]);

  tunfd = tun_alloc(argv[1]);

  printf("Faire la configuration de %s...\n",argv[1]);  

  tun_load_config(argv[1]);
  //tun_copy_output(tunfd, 1);

  ext_in("localhost", tunfd);

  printf("Appuyez sur une touche pour terminer\n");
  getchar();

  return 0;
}