
CC = g++ -std=c++11

all: main

main: main.o
	$(CC) -o $@ $^ -O3

%.o: %.cpp
	$(CC) -c $^ -O3

clean:
	rm *.o
