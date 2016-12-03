CC = gcc
LIB = -lncurses

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