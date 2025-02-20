CC = gcc
FLAGS = -Wall -g

run: myShell
	./myShell

leak:
	valgrind --leak-check=full ./myShell

myShell: myShell.o myFunctionsShell.o
	$(CC) $(FLAGS) -o myShell myShell.o myFunctionsShell.o

myShell.o: myShell.c myShell.h
	$(CC) $(FLAGS) -c myShell.c

myFunctionsShell.o: myFunctionsShell.c myFunctionsShell.h
	$(CC) $(FLAGS) -c myFunctionsShell.c

clean:
	rm -f *.o myShell
