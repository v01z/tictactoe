TARGET=	tictactoe

CC=		g++

DEBUG=	-g

OPT=	-O0

WARN=	-Wall

CCFLAGS=$(DEBUG) $(OPT) $(WARN) -pipe

LD=		g++

LDFLAGS=	-export-dynamic

OBJS=    functions.o main.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)
    
main.o: main.cpp header.hpp
	$(CC) -c $(CCFLAGS) main.cpp -o main.o

functions.o: functions.cpp header.hpp
	$(CC) -c $(CCFLAGS) functions.cpp -o functions.o

clean:
	rm -f *.o  $(TARGET)
