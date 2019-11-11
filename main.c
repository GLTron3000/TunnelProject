#include <stdlib.h>
#include <stdio.h>
#include "extremite.h"
#include "iftun.h"
#include "configLoader.h"

int main (int argc, char** argv){
  struct Config *config = malloc(sizeof(struct Config));
  getConfig("config.txt", config);

  int tunfd;
  printConfig(config);
  tunfd = tun_alloc(config->tun);
  tun_load_config(config->tun, config->inip);

  if(argc == 2){
    ext_in(config->outip, tunfd, config->outport);

    printf("Appuyez sur une touche pour terminer\n");
    getchar();
  }else{
    printf("Ecoute\n");

    ext_out(tunfd, config->inport);

    printf("Appuyez sur une touche pour terminer\n");
    getchar();
  }

  

  return 0;
}