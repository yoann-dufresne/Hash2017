
CC = g++ -std=c++11

all: 

splicing: splicing.o
	$(CC) -o $@ $^ -g

%.o: %.cpp
	$(CC) -c $^ -g

clean:
	rm *.o
