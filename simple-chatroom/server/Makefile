OBJS=server.o Clientread.o MysqlOperator.o logger/logger.o
CC=g++
CFLAGS+=-c -Wall -g
LDFLAGS = -pthread -I /usr/include/mysql -L /usr/lib/mysql -lmysqlclient

server:$(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o:%.cc
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) *.o utility/*.o main -r