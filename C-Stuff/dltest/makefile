CFLAGS=-Wall -W
LN=ln -sf

ALL: libdlforktest libmodule1.so libmodule2.so

libdlforktest: libdlforktest.c
	$(CC) -rdynamic -o $@ $^ -ldl

libmodule1.so: module1.o
	$(CC) $(CFLAGS) -shared -Wl,-soname,libmodule1.so -o libmodule1.so $^

module1.o: module1.c
	$(CC) $(CFLAGS) -fpic -c $^

libmodule2.so: module2.o
	$(CC) -shared -Wl,-soname,libmodule2.so -o libmodule2.so $^

module2.o: module2.c
	$(CC) $(CFLAGS) -fpic -c $^
