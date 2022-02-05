CC = gcc

NAME=lz77
TARGET=$(NAME)

CFLAGS = -Wall -Werror -O2 -DNAME=\"$(NAME)\"

all: lz77 lz77cat

lz77: main.o lz77com.o lz77dec.o tree.o bitio.o compat.o getopt.o error.o
	$(CC) -o lz77 main.o lz77com.o lz77dec.o tree.o bitio.o compat.o getopt.o error.o

lz77cat: maincat.o lz77dec.o tree.o bitio.o compat.o getopt.o error.o
	$(CC) -o lz77cat maincat.o lz77dec.o tree.o bitio.o compat.o getopt.o error.o

main.o: main.c bitio.h lz77.h getopt.h
	$(CC) $(CFLAGS) -c main.c

maincat.o: maincat.c bitio.h lz77.h getopt.h
	$(CC) $(CFLAGS) -c maincat.c

lz77com.o: lz77com.c bitio.h tree.h lz77.h
	$(CC) $(CFLAGS) -c lz77com.c

lz77dec.o: lz77dec.c bitio.h tree.h lz77.h
	$(CC) $(CFLAGS) -c lz77dec.c

tree.o: tree.c tree.h
	$(CC) $(CFLAGS) -c tree.c

bitio.o: bitio.c bitio.h
	$(CC) $(CFLAGS) -c bitio.c

compat.o: compat.c compat.h 
	$(CC) $(CFLAGS) -c compat.c

getopt.o: getopt.c getopt.h
	$(CC) $(CFLAGS) -c getopt.c

.PHONY: clean

clean:
	-rm -f *.o lz77 lz77cat
