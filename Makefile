
SRC = main.c
DEPS = $(wildcard deps/*/*.c)
OBJS = $(SRC:.c=.o) $(DEPS:.c=.o)

CFLAGS = -Ideps -std=c99 -Wall -Wextra

BIN ?= clib-validate
PREFIX ?= /usr/local

$(BIN): $(OBJS)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) $(BIN)

install: $(BIN)
	cp -f $(BIN) $(PREFIX)/bin/

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)

.PHONY: clean install uninstall
