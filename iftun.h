#ifndef IFTUN_H
#define IFTUN_H

int tun_alloc(char *dev);
void tun_load_config(char *dev, char *ip);
void tun_copy_output(int src, int dst);

#endif /* IFTUN_H */