CFLAGS = -g -std=c99 -Wall -Wextra -pedantic -w -O3 -DDEBUG
CC = gcc
TARGET = prekladac
SOURCES = ial.h  scaner.h ifj.h parser.h debug.h
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


clean:
	$(RM) *.o
	$(RM) $(TARGET)