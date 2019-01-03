all: countdown

countdown: countdown.cpp
	g++ -O2 -Wall -std=c++14 -o countdown countdown.cpp

clean:
	rm -f *.s *.o countdown *~

