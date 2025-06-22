FROM debian:bookworm-slim
RUN apt-get update && apt-get --yes install gcc && rm -rf /var/lib/apt/lists/*
COPY server.c .
RUN gcc -o server server.c

FROM debian:bookworm-slim
COPY --from=0 ./server ./server
CMD ["./server"]