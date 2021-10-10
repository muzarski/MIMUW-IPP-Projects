PROJECT = similar_lines
SOURCES = main.c parser.c line_representation.c safe_allocs.c safe_allocs.c
OBJECTS = $(SOURCES:.c=.o)
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

.PHONY: clean

$(PROJECT): $(OBJECTS)
	$(CC) -o $@ $^

main.o: main.c parser.h line_representation.h safe_allocs.h
	$(CC) $(CFLAGS) -c $<
line_representation.o: line_representation.c line_representation.h safe_allocs.h
	$(CC) $(CFLAGS) -c $<
parser.o: parser.c parser.h safe_allocs.h
	$(CC) $(CFLAGS) -c $<
safe_allocs.o: safe_allocs.c safe_allocs.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(PROJECT)