# Makefile pour le Projet OpenGL final
CC = g++
CFLAGS = -Wall -Wextra -std=c++17 -Isrc/common -Isrc
LDFLAGS = -lGL -lglfw -lGLEW

SRC = src/main.cpp src/common/GLShader.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = ProjetOpenGL.exe

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(EXEC)

clean:
	rm -f src/*.o src/common/*.o $(EXEC)
