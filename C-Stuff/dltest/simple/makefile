CFLAGS=-Wall -W
LN=ln -sf

ALL: libdltest libctest.so

libdltest: libdltest.c
	$(CC) -rdynamic -o $@ $^ -ldl

libctest.so: ctest1.o ctest2.o
	$(CC) -shared -Wl,-soname,libctest.so.1 -o libctest.so.1.0 *.o
	$(LN) libctest.so.1.0 libctest.so
	$(LN) libctest.so.1.0 libctest.so.1

ctest2.o: ctest2.c
	$(CC) -Wall -fPIC -c $^

ctest1.o: ctest1.c
	$(CC) -Wall -fPIC -c $^
