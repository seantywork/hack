CC      := gcc
CFLAGS  := -Wall -g
LIBS    := 

SERVER := server.out
CLIENT_TO := client_conn.out


all: $(SERVER) $(CLIENT_TO)


$(SERVER):


	$(CC) $(CFLAGS) -o $(SERVER) server_write_hang.c $(LIBS)

$(CLIENT_TO):

	$(CC) $(CFLAGS) -I. -o $(CLIENT_TO) client_conn.c $(LIBS)


clean:
	rm -f  *.o  *.out 