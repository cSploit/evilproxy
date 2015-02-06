CXX=clang++
CFLAGS=--std=c++14
LDLIBS=-lpthread
SOURCES=$(wildcard *.cpp net/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=evilproxy

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $^ -o $@ $(LDLIBS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CFLAGS)

clean:
	rm $(EXECUTABLE) $(OBJECTS)
