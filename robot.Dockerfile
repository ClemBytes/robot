FROM debian:bookworm-slim
RUN apt-get update && apt-get --yes install gcc && rm -rf /var/lib/apt/lists/*
COPY listen_and_speak.c .
RUN gcc -o listen_and_speak listen_and_speak.c
ENTRYPOINT ./listen_and_speak