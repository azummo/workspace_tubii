all: libtest.a
libtest.a: testfunc.o
	ar rcs libtest.a testfunc.o
testfunc.o: testfunc.c
	cc -c testfunc.c -o testfunc.o
clean:
	rm -rf libtest.a testfunc.o
