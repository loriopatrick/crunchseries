CC=gcc

all: build run

build:
	$(CC) -Wall -g -o out/tests.out tests.c quote.c calc.c server.c stats.c calcHandle.c `mysql_config --cflags --libs`

run:
	./out/tests.out