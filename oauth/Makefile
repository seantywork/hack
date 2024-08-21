

GCC_DEV_FLAGS := -Wall -g

GCC_REL_FLAGS := -Wall

GCC_OBJ_FLAGS := -Wall -c

DEP_PACKAGES := libssl-dev libcurl4-openssl-dev

DEP_MONGOOSE := git clone https://github.com/seantywork/mongoose.git

DEP_CJSON := git clone https://github.com/seantywork/cJSON.git

INCLUDES := -I./include -I./vendor -I./vendor/mongoose -I./vendor/cJSON

LIBS := -lpthread -lcurl


OBJS := f_utils.o
OBJS += f_controller.o


DEP_OBJS := mongoose.o
DEP_OBJS += cJSON.o

all: 

	@echo "frank: dev, release"


dep-package:

	apt-get update

	apt-get -y install $(DEP_PACKAGES)

dep-source:

	cd vendor && $(DEP_MONGOOSE)

	cd vendor && $(DEP_CJSON)


dev: $(OBJS) $(DEP_OBJS)

	gcc $(GCC_DEV_FLAGS) $(INCLUDES) -o frankc.out main.c $(OBJS) $(DEP_OBJS) $(LIBS) 


release: $(OBJS) $(DEP_OBJS)

	gcc $(GCC_REL_FLAGS) $(INCLUDES) -o frankc.out main.c $(OBJS) $(DEP_OBJS) $(LIBS) 



f_utils.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o f_utils.o utils/utils.c 

f_controller.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o f_controller.o controller/index.c 


mongoose.o:


	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o mongoose.o vendor/mongoose/mongoose.c 

cJSON.o:

	gcc $(GCC_OBJ_FLAGS) $(INCLUDES) -o cJSON.o vendor/cJSON/cJSON.c 

clean-dep-source:

	rm -r ./vendor/cJSON ./vendor/mongoose

clean:

	rm -r *.o *.out

