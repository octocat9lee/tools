CXXFLAGS = -std=c++11

BIN_SRCS := $(wildcard *.cpp)
BINS := $(BIN_SRCS:.cpp=)

all: $(BINS)

$(BINS):
	g++ $(CXXFLAGS) -o $@ $(filter %.cpp,$^) $(LDFLAGS) -lpthread

heap_sort: heap_sort.cpp

clean:
	rm -f $(BINS)

