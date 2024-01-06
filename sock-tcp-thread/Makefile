CC      := gcc
CFLAGS  := -Wall -std=c99 -g
LIBS    := -lpthread

SERVER_ST := server_st.out
WORKER := worker.o
CLIENT := client.out


all: $(SERVER_ST) $(CLIENT)


$(SERVER_ST):


	$(CC) $(CFLAGS) -c -o $(WORKER) worker.c 

	$(CC) $(CFLAGS) -o $(SERVER_ST) server_st.c $(WORKER) $(LIBS)

$(CLIENT):

	$(CC) $(CFLAGS) -o $(CLIENT) client.c $(LIBS)


clean:
	rm -f  *.o  *.out 0 1 2 3 4 5 10 11