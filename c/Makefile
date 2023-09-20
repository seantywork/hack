
execve-bin-sh-linux-x64: execve-bin-sh/linux-x64/hack.c
	gcc -fno-stack-protector -z execstack -o execve-bin-sh/linux-x64/hack.out execve-bin-sh/linux-x64/hack.c

stack-overflow-linux-x64: stack-overflow/linux-x64/hack.c
	gcc -g -fno-stack-protector -z execstack -no-pie -o stack-overflow/linux-x64/hack.out stack-overflow/linux-x64/hack.c

http-req-debug: http-req/http-req.c
	gcc -g -o http-req/hr.out http-req/http-req.c 

http-req: http-req/http-req.c
	gcc -o http-req/hr.out http-req/http-req.c 
