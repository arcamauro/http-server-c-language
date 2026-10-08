FROM gcc:latest

RUN apt-get update && apt-get install -y make

WORKDIR /http-server-c-language

COPY . .

EXPOSE 8080

RUN make

CMD ["./server"]

