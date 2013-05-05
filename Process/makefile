CC=gcc

all: build run

build:
	$(CC) -Wall -g -o out/tests.out tests.c quote.c network.c netHandler.c database.c stats.c `mysql_config --cflags --libs`

run:
	./out/tests.out