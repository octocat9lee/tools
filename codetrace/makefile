CXXFLAGS = -g -finstrument-functions

code_trace:
	g++ $(CXXFLAGS) main.cpp code_trace.cpp -o $@

clean:
	rm -f code_trace code_trace.out
