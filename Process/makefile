CC=gcc

all: build run

build:
	$(CC) -Wall -g -o out/tests.out tests.c quote.c qStreamCalc.c network.c qStreamCalcNetHandler.c qStreamStats.c database.c `mysql_config --cflags --libs`

run:
	./out/tests.out