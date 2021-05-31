CC = gcc
CFLAGS = -m32 -fno-stack-protector -w -no-pie -O0

hellomake:
	$(CC) app1.c $(CFLAGS) -Wl,-z,norelro -o app1.o
	$(CC) app1.c $(CFLAGS) -o app1-partial.o
	$(CC) app1.c $(CFLAGS) -Wl,-z,relro,-z,now -o app1-full.o

clean:
	rm -f app1.o app1-partial.o app1-full.o
