CC=clang

build:
	$(CC) -Wall -g tests.c quote.c calc.c server.c stats.c calcHandle.c -Ilib/mysql/include -o out/tests.out `mysql_config --cflags --libs`
	./out/tests.out