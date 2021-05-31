CC = gcc
CFLAGS = -no-pie -m32 -z execstack -w

hellomake:
	$(CC) app1.c $(CFLAGS) -o app1.o
	$(CC) app1.c $(CFLAGS) -fstack-protector -o app1-canary.o
	$(CC) app2.c $(CFLAGS) -fstack-protector -o app2-canary.o
	$(CC) gcc-test.c -o gcc-test.o

clean:
	rm -f app1.o app1-canary.o app2-canary.o gcc-test.o
