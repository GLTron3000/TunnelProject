#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

struct Config {
	char tun[50];
	char inip[50];
	char inport[50];
	char options[50];
	char outip[50];
	char outport[50];
};

void getConfig(char nameFile[30], struct Config *config);
void printConfig(struct Config *config);

#endif /* CONFIGLOADER_H */