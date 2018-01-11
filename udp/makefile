CXXFLAGS = -O0 -g -finstrument-functions -Wall -Werror

all:
	g++ $(CXXFLAGS) udp_client.cpp -o udp_client
	g++ $(CXXFLAGS) udp_server.cpp -o udp_server

clean:
	rm -f udp_client udp_server
