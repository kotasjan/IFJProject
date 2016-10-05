CFLAGS = -g -std=c99 -Wall -Wextra -pedantic -w -O3 
CC = gcc
TARGET = prekladac
SOURCES = ial.h  scaner.h ifj.h parser.h
OBJECTS = $(SOURCES:.h=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	gcc $^ -o $@

ifj.o: ifj.c ial.h ifj.h
	$(CC) $(CFLAGS) -c $< -o $@

ial.o: ial.c ial.h scaner.h
	$(CC) $(CFLAGS) -c $< -o $@

scaner.o: scaner.c scaner.h
	$(CC) $(CFLAGS) -c $< -o $@

parser.o: parser.c parser.h ial.h
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	$(RM) *.o
	$(RM) $(TARGET)