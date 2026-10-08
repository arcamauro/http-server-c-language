# HTTP server in C

## Introduction
A lightweight Linux HTTP server using nonblocking sockets and `epoll`. It features graceful shutdown and Dynamic Static File Resolution.

## Architecture
```ascii
├── Dockerfile
├── html
│   ├── example.html
│   └── index.html
├── include
│   ├── event_loop.h
│   ├── http.h
│   ├── sig_utils.h
│   └── socket_utils.h
├── Makefile
├── README.md
└── src
    ├── event_loop.c
    ├── http.c
    ├── server.c
    ├── sig_utils.c
    └── socket_utils.c
```

## Sources:
```ascii
├── event_loop.c // for epoll
├── http.c // for http responses
├── server.c // entry point
├── sig_utils.c // for graceful shutdown
└── socket_utils.c // for socket handling
```

## Build and Run
### Makefile
Build and run with a C compiler and Make:

```sh
make
./server
```

Run `make clean` to remove build artifacts. 

### Docker
To build and run with Docker:

```sh
docker build -t http-server-c-language .
```

(to run if you want detached mod add flag -d)
```sh
docker run -p 8080:8080 --name my-c-server http-server-c-language
```

To view logs
```sh 
docker logs my-c-server
```

To stop running a container and trigger SIGTERM
```sh
docker stop my-c-server
```
To start an existing container
```sh
docker start my-c-server
```
To remove an existing container
```sh
docker rm my-c-server
```
To see how it works go to `http://localhost:8080/`, it will open `index.html`. Go to `http://localhost:8080/example.html` to open the other page.

### References
[Guide for Network Programming in C](https://beej.us/guide/bgnet/html/)
