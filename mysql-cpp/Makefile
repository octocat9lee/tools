LIBS=-L/usr/lib64/mysql -lmysqlclient -lstdc++
CXXFLAGS=-std=c++11 -O0 -g

.PHONY: all clean check

all:

check: test
	./test

test: test.o
	gcc $^ -o $@ $(LIBS)

.o:.cpp
	gcc $< -o $@ $(CXXFLAGS)

test.o: test.cpp
test.cpp: mysqlcpp.h

clean:
	rm -rf test
	rm -rf test.o
