CC = cc
CPPFLAGS ?=
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic
LDFLAGS ?=
LDLIBS ?=

SOURCES = src/server.c src/socket_utils.c src/event_loop.c src/http.c src/sig_utils.c
OBJECTS = $(patsubst src/%.c,build/%.o,$(SOURCES))
DEPS = $(OBJECTS:.o=.d)

.PHONY: all clean
all: server

server: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

build/%.o: src/%.c | build
	$(CC) $(CPPFLAGS) -Iinclude $(CFLAGS) -MMD -MP -c $< -o $@

build:
	mkdir -p $@

clean:
	$(RM) $(OBJECTS) $(DEPS) server

-include $(DEPS)
