OBJS	= Testing.o
SOURCE	= Testing.cpp
HEADER	= 
OUT	= Testing
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

Testing.o: Testing.cpp
	$(CC) $(FLAGS) Testing.cpp -std=c++11


clean:
	rm -f $(OBJS) $(OUT)
