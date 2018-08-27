all: autobuf-test

autobuf-test: autobuf.c autobuf.h autobuf-test.c
	gcc -o autobuf-test autobuf.c autobuf-test.c -O2 -g

clean:
	rm -f autobuf-test

