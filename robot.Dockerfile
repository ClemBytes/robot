FROM debian:bookworm-slim AS gcc
RUN apt-get update && apt-get --yes install gcc && rm -rf /var/lib/apt/lists/*
COPY server.c .
RUN gcc -o server server.c

FROM debian:bookworm-slim
COPY --from=gcc ./server ./server
CMD ["./server"]