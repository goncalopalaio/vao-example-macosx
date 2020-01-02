# Flags
CFLAGS = -pedantic -Wno-deprecated
LIBS :=
CC= g++
OBJS =

ifeq ($(OS),Windows_NT)
BIN := $(BIN).exe
	LIBS := -lglfw -lopengl32 -lm -lGLU32 -lGLEW32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LIBS := -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm -lGLEW -L/usr/local/lib
		CFLAGS += -I/usr/local/include
	else
		LIBS := -lglfw -lGL -lm -lGLU -lGLEW
	endif
endif

all: generate main

generate: clean
ifeq ($(OS),Windows_NT)
	@mkdir bin 2> nul || exit 0
else
	@mkdir -p bin	
endif

clean:
	@rm -rf bin

main: generate ${OBJS} 
	$(CC) $(CFLAGS) $(OBJS) -o main main.cpp $(LIBS)

g_assimp_loader.o: g_assimp_loader.cpp g_assimp_loader.h
