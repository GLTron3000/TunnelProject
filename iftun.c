#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

int tun_alloc(char *dev){
  struct ifreq ifr;
  int fd, err;

  if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ){
    perror("alloc tun");
    exit(-1);
  }

  memset(&ifr, 0, sizeof(ifr));

  /* Flags: IFF_TUN   - TUN device (no Ethernet headers) 
   *        IFF_TAP   - TAP device  
   *
   *        IFF_NO_PI - Do not provide packet information  
   */ 
  ifr.ifr_flags = IFF_TUN; 
  if( *dev )
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);

  if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ){
    close(fd);
    return err;
  }
  strcpy(dev, ifr.ifr_name);
  return fd;
}      

void tun_load_config(char *dev){
  char strUp[80];
  strcpy(strUp, "ip link set ");
  strcpy(strUp, dev);
  strcpy(strUp, " up");

  system(strUp);


  char strAddr[80];
  strcpy(strAddr, "ip addr add 172.16.2.1/24 dev ");
  strcpy(strAddr, dev);
  system(strAddr);
}

void tun_copy_output(int src, int dst){
    char buffer[20];
    size_t nbytes;
    ssize_t bytes_read;

    nbytes = sizeof(buffer);
    
    do{
        bytes_read = read(src, buffer, nbytes);
        write(dst, buffer, nbytes);
    }
    while(bytes_read != 0);
}