CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++17
PKG_LIBS := -lGL -lGLU -lglut

SRCS := $(wildcard *.cpp)
EXES := $(SRCS:.cpp=.exe)

.PHONY: all clean

all: $(EXES)

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(PKG_LIBS)

clean:
	rm -f $(EXES)