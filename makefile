CFLAGS = -g -std=c99 -Wall -Wextra -pedantic -w -O3 -DDEBUG
CC = gcc
TARGET = prekladac
SOURCES = ial.h  scaner.h ifj.h debug.h parse_new.h ifj16Func.h expr.h interpret.h instruction.h
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

scaner.o: scaner.c scaner.h debug.h
	$(CC) $(CFLAGS) -c $< -o $@

parser.o: parser.c parser.h ial.h debug.h
	$(CC) $(CFLAGS) -c $< -o $@

parse_new.o: parse_new.c parse_new.h ial.h debug.h ifj16Func.h
	$(CC) $(CFLAGS) -c $< -o $@

ifj16Func.o: ifj16Func.c ifj16Func.h ial.h ifj.h debug.h
	$(CC) $(CFLAGS) -c $< -o $@

expr.o: expr.c expr.h ial.h scaner.h debug.h
	$(CC) $(CFLAGS) -c $< -o $@

interpret.o: interpret.c interpret.h ifj.h instruction.h
	$(CC) $(CFLAGS) -c $< -o $@

instruction.o: instruction.c instruction.h 
	$(CC) $(CFLAGS) -c $< -o $@





clean:
	$(RM) *.o
	$(RM) $(TARGET)