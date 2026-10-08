FROM gcc:latest

RUN apt-get update && apt-get install -y cmake make gdb

WORKDIR /http-server-c-language

COPY . .

EXPOSE 8080

RUN make

CMD ["./server"]

