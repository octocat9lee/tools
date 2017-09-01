NDEBUG=0
STATIC=0
SHARED=1
LCOV=0

TARGETDIR=../lib
NAME=libshared.so

SRC+=$(shell find ./ -name "*.cpp")

#INC=-I./

#LIB_DIR+=./

CXXFLAGS+=-Wall -Wno-deprecated -Wno-unused-function

include ../version.pub #must before in makefile.pub
include ../makefile.pub

