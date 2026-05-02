CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17
PKG_LIBS := -lGL -lglfw -lGLEW

COMMON_SRCS := common/GLShader.cpp
SRCS := $(wildcard *.cpp)
EXES := $(SRCS:.cpp=.exe)

.PHONY: all clean

all: $(EXES)

%.exe: %.cpp $(COMMON_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(PKG_LIBS)

run: clean main.exe
	DISPLAY=:1 ./main.exe

clean:
	rm -f $(EXES)