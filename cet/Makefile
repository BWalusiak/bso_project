CC = gcc
CFLAGS = -m32 -fno-stack-protector -w

hellomake:
	$(CC) app1.c $(CFLAGS) -static -o app1-static.o
	$(CC) app1.c $(CFLAGS) -fcf-protection -o app1-cet.o 
clean:
	rm -f app1-cet.o app1-static.o
