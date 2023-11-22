all:
	@ echo "ws"

clean:

	rm -rf *.o

	rm -rf *.out

server:

	gcc -IwsServer/include -LwsServer -o serve.out main.c -lws

client:

	gcc -c -IwsServer/extra/toyws -o toyws.o wsServer/extra/toyws/toyws.c 

	gcc -IwsServer/extra/toyws -o req.out req.c toyws.o

server-debug:

	gcc -g -IwsServer/include -LwsServer -o serve.out main.c -lws

client-debug:

	gcc -g -c -IwsServer/extra/toyws -o toyws.o wsServer/extra/toyws/toyws.c 

	gcc -g -IwsServer/extra/toyws -o req.out req.c toyws.o