hellomake:
	gcc app1.c -no-pie -m32 -w -o app1.o
	gcc app1.c -m32 -w -o app1-pie.o
	gcc app1.c -m32 -static -w -o app1-static.o

clean:
	rm -f app1.o app1-pie.o app1-static.o
