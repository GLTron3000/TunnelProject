SHELL   = /bin/bash
CC      = gcc
MKDEP   = gcc -MM
RM      = rm -f

CFLAGS  = -Wall -std=gnu99 -pedantic -O2
LIBS    = -lpthread

CFILES  = main.c extremite.c iftun.c configLoader.c

EXEC    = tunnel46d


OBJECTS := $(CFILES:%.c=%.o)

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c

all :: $(EXEC)

$(EXEC) : $(OBJECTS)
	$(CC) -o $@ $^ $(LIBS)

clean ::
	$(RM) *.o *~ $(EXEC) depend

depend : *.c *.h
	$(MKDEP) *.c >| depend


-include depend
