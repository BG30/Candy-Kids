TARGET = candykids

candykids: bbuff.o stats.o candykids.o
	$(CC) $(CFLAGS) $(OBJS) -lrt -lpthread -o $(TARGET)

OBJS = bbuff.o stats.o candykids.o

bbuff.o: bbuff.c
	$(CC) bbuff.c -c

stats.o : stats.c
	$(CC) stats.c -c 

candykids.o : candykids.c
	$(CC) candykids.c -c

CC = gcc
CFLAGS = -Wall -g -std=c99 -D_POSIX_C_SOURCE=199309L 

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)