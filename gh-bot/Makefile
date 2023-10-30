
all: 
	@echo "github cli bot"

dep:

	git clone https://github.com/seantywork/cJSON.git

	cd cJSON && sudo mkdir -p /usr/local/include/cjson && sudo /bin/cp -Rf cJSON.h /usr/local/include/cjson/cJSON.h &&  /bin/cp -Rf cJSON.h ../cJSON.h && /bin/cp -Rf cJSON.c ../cJSON.c

	sudo rm -r cJSON

	gcc -c -o cJSON.o cJSON.c


test:

	gcc -o var.out var.c cJSON.o
	