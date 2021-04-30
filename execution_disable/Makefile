hellomake:
	gcc app1.c -no-pie -m32 -z execstack -w -o app1.o
	gcc app1.c -no-pie -m32 -w -o app1-nx.o

clean:
	rm -f app1.o app1-nx.o
