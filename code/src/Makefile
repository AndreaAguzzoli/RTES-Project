OBJS	= Testing.o
SOURCE	= Testing.cpp
HEADER	= 
OUT	= Testing
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread
INCLUDE := $(shell python3.8 -m pybind11 --includes)
PY_FLAGS    := -shared -std=c++11 -fPIC
SUFFIX  := .so
LDFLAGS := $(shell python3.8-config --ldflags)
OTHER   := -I./pybind11/include/

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

Testing.o: Testing.cpp
	$(CC) $(FLAGS) Testing.cpp -std=c++11

Queue_cpp_int.so:
	g++ $(PY_FLAGS) $(OTHER) $(INCLUDE) int_pywrap.cpp -o Queue_cpp_int$(SUFFIX) $(LDFLAGS)

Queue_cpp_double.so:
	g++ $(PY_FLAGS) $(OTHER) $(INCLUDE) double_pywrap.cpp -o Queue_cpp_double$(SUFFIX) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(OUT)
