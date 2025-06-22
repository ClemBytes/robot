FROM debian:bookworm-slim AS gcc
RUN apt-get update && apt-get --yes install gcc && rm -rf /var/lib/apt/lists/*
COPY server.c .
RUN gcc -static -o server server.c

FROM scratch
COPY --from=gcc ./server ./server
CMD ["./server"]