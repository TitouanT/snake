CC = gcc
LIB = -lncurses

snake: main.o list_ptr.o
	$(CC) $^ -o $@ $(LIB)

main.o: main.c list_ptr.h
	$(CC) -c $<

list_ptr.o: list_ptr.c list_ptr.h
	$(CC) -c $<

clean:
	rm *.o snake

clear: clean

mrproper: clean snake