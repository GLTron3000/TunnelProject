#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "extremite.h"
#include "iftun.h"
#include "configLoader.h"

int main (int argc, char** argv){
  if(argc == 1 || (argc == 3 && strcmp(argv[2], "-server") != 0) || argc > 3){
    printf("Usage : # ./tunnel46d config.txt [-server]\n");
    exit(0);
  }

  struct Config *config = malloc(sizeof(struct Config));
  getConfig(argv[1], config);

  int tunfd;
  printConfig(config);
  tunfd = tun_alloc(config->tun);
  tun_load_config(config->tun, config->inip);

  if(argc == 3){
    printf("Ecoute...\n");

    ext_out(tunfd, config->inport);
  }else{
    printf("Connection...\n");

    ext_in(config->outip, tunfd, config->outport);
  }

  return 0;
}