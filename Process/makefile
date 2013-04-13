CC=clang

build:
	$(CC) -Wall -g tests.c quote.c calc.c server.c stats.c -Ilib/mysql/include -o out/tests.out `mysql_config --cflags --libs`
	./out/tests.out