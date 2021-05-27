hellomake:
	gcc app1.c -m32 -static -w -o app1-static.o
	gcc app1.c -m32 -w -o app1-cet.o -fcf-protection

clean:
	rm -f app1.o app1-pie.o app1-static.o
