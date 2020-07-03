CC=cc
CFLAGS=-std=c99 -o ssm

ssm: main.c ansicodes.h
	@$(CC) $(CFLAGS) main.c

install:
	@install -s ssm /bin/ssm

clean:
	@rm ssm

uninstall:
	@rm /bin/ssm
