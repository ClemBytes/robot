FROM debian:bookworm
RUN apt-get update
RUN apt-get --yes install gcc
COPY listen_and_speak.c .
RUN gcc -o listen_and_speak listen_and_speak.c
ENTRYPOINT ./listen_and_speak