


INCLUDES := -I.
LIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all:
	@echo "raylib game-engine"

build:

	gcc $(INCLUDES) -o run.out game.c $(LIBS)

desktop:

	cd raylib/src/ && make PLATFORM=PLATFORM_DESKTOP

	cd raylib/src/ && sudo make install 

desktop-shared:

	cd raylib/src/ &&  make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED

	cd raylib/src/ &&  sudo make install RAYLIB_LIBTYPE=SHARED

web:

	cd raylib/src/ &&  make PLATFORM=PLATFORM_WEB

uninstall:

	cd raylib/src/ &&  sudo make uninstall

uninstall-shared:

	cd raylib/src/ &&  sudo make uninstall RAYLIB_LIBTYPE=SHARED

clean:

	cd raylib/src/ &&  make clean

	rm -rf *.out