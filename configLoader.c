#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "configLoader.h"


void getConfig(char nameFile[30], struct Config *config){
	
	FILE* file = NULL;
	
	char chemin[50];
	sprintf(chemin, "%s", nameFile);
	
	file = fopen(chemin, "r");
	char delim[] = "=";
	char *ptr;
	
	if(file != NULL){
	
		char line[30];
		
		//Ignore premiere ligne
		fgets(line, 30, file);

		//Recup nom tun0
		fgets(line, 30, file);
		ptr = strtok(line, delim);
		ptr = strtok(NULL, delim);
		sprintf(config->tun, "%s", ptr);

		//Ignore 3eme ligne
		fgets(line, 30, file);

		//Lecture adresse ip locale
		fgets(line, 30, file);
		ptr = strtok(line, delim);
		ptr = strtok(NULL, delim);
		sprintf(config->inip, "%s", ptr);
		
		//Lecture port local
		fgets(line, 30, file);
		ptr = strtok(line, delim);
		ptr = strtok(NULL, delim);
		sprintf(config->inport, "%s", ptr);

		//Lecture options
		fgets(line, 30, file);
		ptr = strtok(line, delim);
		ptr = strtok(NULL, delim);
		sprintf(config->options, "%s", ptr);
		
		//Ignore 7eme ligne
		fgets(line, 30, file);		
		
		//Lecture afresse ip distante
		fgets(line, 30, file);
		ptr = strtok(line, delim);
		ptr = strtok(NULL, delim);
		sprintf(config->outip, "%s", ptr);

		//Lecture port distant
		fgets(line, 30, file);
		ptr = strtok(line, delim);
		ptr = strtok(NULL, delim);
		sprintf(config->outport, "%s", ptr);

	}
}

void printConfig(struct Config *config){
    printf("Config : \n Interface: %s\n Local ip: %s Local Port: %s\n Distant ip: %s Distant Port: %s\n",config->tun, config->inip, config->inport, config->outip, config->outport);
}