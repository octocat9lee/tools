NDEBUG=0
STATIC=1
SHARED=0
LCOV=0

TARGETDIR=../lib
NAME=libfacade.a

SRC+=$(shell find ./ -name "*.cpp")

INC=-I./ -I../

LIB_DIR+=-L../lib

STAT_LIB+=-llog4cplus
#LIB+=

CXXFLAGS+=-Wall -Wshadow -Werror -Wunreachable-code

include ../makefile.pub

