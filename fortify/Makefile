CC = gcc
CFLAGS = -m32 -fno-stack-protector -w -g -O1

hellomake:
	$(CC) app1.c $(CFLAGS) -D_FORTIFY_SOURCE=2 -std=c99 -o app1.o
	$(CC) app2.c $(CFLAGS) -D_FORTIFY_SOURCE=1 -o app2-level1.o
	$(CC) app2.c $(CFLAGS) -D_FORTIFY_SOURCE=2 -o app2-level2.o
	$(CC) app3.c $(CFLAGS) -D_FORTIFY_SOURCE=2 -o app3.o

clean:
	rm -f app1.o app1-level1.o app2-level2.o app3.o
