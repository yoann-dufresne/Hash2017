
CC = g++ -std=c++11

all: main

main: main.o
	$(CC) -o $@ $^ -g

%.o: %.cpp
	$(CC) -c $^ -g

clean:
	rm *.o
