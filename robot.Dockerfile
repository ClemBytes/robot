FROM debian:bookworm-slim AS gcc
RUN apt-get update && apt-get --yes install gcc make && rm -rf /var/lib/apt/lists/*
COPY server.c .
COPY base64.c .
COPY base64.h .
COPY Makefile .
RUN make LDFLAGS+=-static server

FROM scratch
COPY --from=gcc ./server ./server
COPY data data
CMD ["./server"]