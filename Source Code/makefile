Unix_Shell: Unix_Shell.o token.o command.o builtin.o
	gcc Unix_Shell.o token.o command.o builtin.o -o Unix_Shell

Unix_Shell.o: Unix_Shell.c token.h command.h builtin.h
	gcc -c -Wall Unix_Shell.c

buildin.o: builtin.c builtin.h
	gcc -c -Wall builtin.c

command.o: command.c command.h
	gcc -c -Wall command.c

token.o: token.c token.h
	gcc -c -Wall token.c

clean:
	rm *.o
