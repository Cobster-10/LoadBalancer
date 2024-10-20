CC = g++
CFLAGS = -std=c++11 -Wall -Wextra -Werror -pedantic -O3

OBJFILES = main.o LoadBalancer.o WebServer.o

myprogram: $(OBJFILES)
	$(CC) $(CFLAGS) -o myprogram $(OBJFILES)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

LoadBalancer.o: LoadBalancer.cpp LoadBalancer.h
	$(CC) $(CFLAGS) -c LoadBalancer.cpp

WebServer.o: WebServer.cpp WebServer.h
	$(CC) $(CFLAGS) -c WebServer.cpp

clean:
	rm -f myprogram $(OBJFILES)