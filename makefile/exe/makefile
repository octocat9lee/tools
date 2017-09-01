NDEBUG=0
STATIC=0
SHARED=0
LCOV=0

TARGETDIR=../bin
NAME=exe

SRC+=$(shell find ./ -name "*.cpp")

INC=-I./ \
    -I../static \
    -I../shared

LIB_DIR+=-L../lib

LIB+=-lstatic -lshared

CXXFLAGS+=-Wall -Wno-deprecated -Wno-unused-function

include ../makefile.pub

