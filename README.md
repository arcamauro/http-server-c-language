# HTTP server in C

A Linux HTTP server using nonblocking sockets and epoll. It listens on port
8080 and responds with `Example` followed by a newline.

Build and run with a C compiler and Make:

```sh
make
./server
```

In another terminal:

```sh
curl http://localhost:8080/
```

Run `make clean` to remove build artifacts. To build and run with Docker:

```sh
docker build -t http-server .
docker run --rm -p 8080:8080 http-server
```

## Source layout

- `server.c`: entry point and server configuration.
- `socket_utils.c` / `socket_utils.h`: nonblocking socket setup and listener creation.
- `event_loop.c` / `event_loop.h`: epoll registration and connection acceptance.
- `http.c` / `http.h`: request reading, fixed HTTP response, and client cleanup.

This refactor preserves the existing one-read, one-send request handling;
request parsing and buffered partial writes are not implemented.
