CC = gcc
LIB = $(shell pkg-config --libs ncursesw 2>/dev/null)
ifeq ($(LIB),)
LIB = -lncurses
endif

snake: main.o list_ptr.o
	$(CC) $^ -o $@ $(LIB) -Wall

main.o: main.c list_ptr.h
	$(CC) -c $< -Wall

list_ptr.o: list_ptr.c list_ptr.h
	$(CC) -c $< -Wall

clean:
	rm *.o snake

clear: clean

mrproper: clean snake
