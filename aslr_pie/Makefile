CC = gcc
CFLAGS = -m32 -fno-stack-protector -w

hellomake:
	$(CC) app1.c $(CFLAGS) -no-pie -o app1.o
	$(CC) app1.c $(CFLAGS) -o app1-pie.o
	$(CC) app1.c $(CFLAGS) -static -o app1-static.o

clean:
	rm -f app1.o app1-pie.o app1-static.o
