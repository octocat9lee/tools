NDEBUG=0
STATICA=0
SHARED=0
LCOV=0

TARGETDIR=./
NAME=log4cplus_facade_test

SRC+=$(shell find ./ -name "*.cpp")

INC=-I./ \
    -I../ \
    -I../src

LIB_DIR+=-L../lib

STAT_LIB+=-lfacade -lrt -llog4cplus
LIB+=-lpthread

CXXFLAGS+=-Wall -Wshadow -Werror -Wunreachable-code

MACRO+=-D__STDC_FORMAT_MACROS

include ../makefile.pub

