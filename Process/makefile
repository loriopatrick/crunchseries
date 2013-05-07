CC=clang

all: build run

build:
	$(CC) -g -Wall -o out/tests.out tests.c quote.c network.c netHandler.c database.c stats.c statGraph.c `mysql_config --cflags --libs`

run:
	./out/tests.out