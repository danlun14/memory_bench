all: main

main: main.c CLI.c
	gcc -omain main.c CLI.c hpctimer.c -Wall -lm