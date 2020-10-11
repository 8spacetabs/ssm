CC=cc
CFLAGS=-std=c99 -o ssm

tags: main.c ansicodes.h
	@$(CC) $(CFLAGS) main.c

install:
	@install -s ssm /bin/tags

clean:
	@rm tags

uninstall:
	@rm /bin/tags
