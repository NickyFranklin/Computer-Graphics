FLAGS= -g -Wall -Wextra -std=gnu99

ray1: main.o
	gcc ${FLAGS} -fPIE -o ray1 main.o -lm
main.o: main.c
clean:
	rm -f ray1
