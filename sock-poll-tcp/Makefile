CC      := gcc
CFLAGS  := -Wall -g
LIBS    := 

SERVER_P := server_p.out
CLIENT := client.out


all: $(SERVER_P) $(CLIENT)


$(SERVER_P):


	$(CC) $(CFLAGS) -I. -o $(SERVER_P) server_p.c $(LIBS)

$(CLIENT):

	$(CC) $(CFLAGS) -o $(CLIENT) client.c $(LIBS)


clean:
	rm -f  *.o  *.out 