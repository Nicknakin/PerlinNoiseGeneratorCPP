CXX=g++
CXXFLAGS=-std=c++17 -Wall -I include/
LDLIBS=-lsfml-graphics -lsfml-window -lsfml-system -pthread
SRC_DIR=./
OBJ_DIR=./
BUILD_DIR=./build

BRACKET_STYLE=java

OUTPUT=NoiseDisplay

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

.DEFAULT: all
.PHONY: all

all: ${OBJ_FILES}


{OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	${CXX} ${CXXFLAGS} -c $< -o ${OBJ_DIR}/$@ ${LDLIBS}

preprocess: CXXFLAGS+=-E
preprocess: default

debug: CXXFLAGS+=-g 
debug: clean all 
prettify:
	-find -type f \( -iname \*.h -o -iname \*.hpp -o -iname \*.cpp \) -exec astyle -n --style=${BRACKET_STYLE} {} \;

clean:
	-rm -rf *.o	
