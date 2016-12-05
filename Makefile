CFLAGS = -g -std=c99 -Wall -Wextra -pedantic -w -O3 
CC = gcc
TARGET = prekladac
SOURCES = ial.h  scaner.h ifj.h debug.h parse_new.h ifj16func.h expr.h interpret.h instruction.h tokenfifo.h
OBJECTS = $(SOURCES:.h=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	gcc $^ -o $@

debug.o: debug.c scaner.h debug.h ifj.h
	$(CC) $(CFLAGS) -c $< -o $@

ifj.o: ifj.c ial.h ifj.h debug.h
	$(CC) $(CFLAGS) -c $< -o $@

ial.o: ial.c ial.h scaner.h debug.h
	$(CC) $(CFLAGS) -c $< -o $@

scaner.o: scaner.c scaner.h debug.h tokenfifo.h
	$(CC) $(CFLAGS) -c $< -o $@

parser.o: parser.c parser.h ial.h debug.h
	$(CC) $(CFLAGS) -c $< -o $@

parse_new.o: parse_new.c parse_new.h ial.h debug.h ifj16func.h expr.h tokenfifo.h
	$(CC) $(CFLAGS) -c $< -o $@

ifj16func.o: ifj16func.c ifj16func.h ial.h ifj.h debug.h
	$(CC) $(CFLAGS) -c $< -o $@

expr.o: expr.c expr.h ial.h scaner.h debug.h parse_new.h
	$(CC) $(CFLAGS) -c $< -o $@

interpret.o: interpret.c interpret.h ifj.h instruction.h
	$(CC) $(CFLAGS) -c $< -o $@

instruction.o: instruction.c instruction.h 
	$(CC) $(CFLAGS) -c $< -o $@

sem.o: sem.c sem.h ial.h debug.h instruction.h instruction.h parse_new.h
	$(CC) $(CFLAGS) -c $< -o $@ 

tokenfifo.o: tokenfifo.c tokenfifo.h ial.h debug.h scaner.h
	$(CC) $(CFLAGS) -c $< -o $@ 





clean:
	$(RM) *.o
	$(RM) $(TARGET)

test:
	./test.sh
