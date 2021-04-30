hellomake:
	gcc app1.c -no-pie -m32 -z execstack -w -o app1.o
	gcc app1.c -no-pie -m32 -z execstack -fstack-protector -w -o app1-canary.o
	gcc app2.c -no-pie -m32 -z execstack -fstack-protector -w -o app2-canary.o
	gcc gcc-test.c -o gcc-test.o

clean:
	rm -f app1.o app1-canary.o app2-canary.o gcc-test.o
