CC=gcc
CXX=g++

CFLAGS=-c -O2 -MMD -MP -Wstrict-prototypes
CXXFLAGS=-c -O2 -MMD -MP
LDFLAGS=-lpulse-simple -lncurses -lpthread

BUILD_DIR=build
BINARY=${BUILD_DIR}/analyzer

SRC_FILES=$(shell find . -name \*.c -or -name \*.cpp)
OBJ_FILES=$(SRC_FILES:%=${BUILD_DIR}/%.o)
DEP_FILES=${OBJ_FILES:.o=.d}

all: ${BINARY}

${BINARY}: ${OBJ_FILES}
	${CXX} ${LDFLAGS} $^ -o $@

${BUILD_DIR}/%.c.o: %.c
	mkdir -p ${BUILD_DIR}
	${CC} ${CFLAGS} $< -o $@

${BUILD_DIR}/%.cpp.o: %.cpp
	mkdir -p ${BUILD_DIR}
	${CXX} ${CXXFLAGS} $< -o $@

-include ${DEP_FILES}

clean:
	rm -rf ${BUILD_DIR}

.PHONY: all, clean
