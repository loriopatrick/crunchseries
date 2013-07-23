CC=clang

all: build run

build:
	$(CC) -g -Wall -o out/tests.out main.c quote.c network.c netHandler.c database.c stats.c statGraph.c arithmetic_stats.c logic_stats.c -lm `mysql_config --cflags --libs`
run:
	./out/tests.out