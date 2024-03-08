CC      := gcc
CFLAGS  := -Wall -std=c99 -g
LIBS    := 

SERVER_S := server_s.out
CLIENT := client.out


all: $(SERVER_S) $(CLIENT)


$(SERVER_S):

	$(CC) $(CFLAGS) -o $(SERVER_S) server_s.c $(LIBS)

$(CLIENT):

	$(CC) $(CFLAGS) -o $(CLIENT) client.c $(LIBS)


clean:
	rm -f  *.o  *.out