FLAGS= -g -Wall -Wextra -std=gnu99

ray2: main.o
	gcc ${FLAGS} -fPIE -o ray2 main.o -lm
main.o: main.c
clean:
	rm -f ray2
