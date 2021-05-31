CC = gcc
CFLAGS = -fno-stack-protector -no-pie -m32 -w
hellomake:
	$(CC) app1.c $(CFLAGS) -z execstack -o app1.o
	$(CC) app1.c $(CFLAGS) -o app1-nx.o

clean:
	rm -f app1.o app1-nx.o
