CC = cc
CPPFLAGS ?=
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic
LDFLAGS ?=
LDLIBS ?=

SOURCES = server.c socket_utils.c event_loop.c http.c sig_utils.c
OBJECTS = $(SOURCES:.c=.o)
DEPS = $(OBJECTS:.o=.d)

.PHONY: all clean
all: server

server: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	$(RM) $(OBJECTS) $(DEPS) server

-include $(DEPS)
