all:

	@echo "mqtt"


build: 

	gcc -c -IMQTT-C/include -IMQTT-C/examples -o mqtt.o MQTT-C/src/mqtt.c 

	gcc -c -IMQTT-C/include -IMQTT-C/examples -o mqtt_pal.o MQTT-C/src/mqtt_pal.c 

	gcc -IMQTT-C/include -IMQTT-C/examples -o pub.out pub.c mqtt.o mqtt_pal.o -lpthread

	gcc -IMQTT-C/include -IMQTT-C/examples -o sub.out sub.c mqtt.o mqtt_pal.o -lpthread

build-debug:

	gcc -g -c -IMQTT-C/include -IMQTT-C/examples -o mqtt.o MQTT-C/src/mqtt.c 

	gcc -g -c -IMQTT-C/include -IMQTT-C/examples -o mqtt_pal.o MQTT-C/src/mqtt_pal.c 

	gcc -g -IMQTT-C/include -IMQTT-C/examples -o pub.out pub.c mqtt.o mqtt_pal.o -lpthread

	gcc -g -IMQTT-C/include -IMQTT-C/examples -o sub.out sub.c mqtt.o mqtt_pal.o -lpthread


clean:

	rm *.out

	rm *.o


