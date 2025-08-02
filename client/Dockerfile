# build stage
FROM debian:bullseye AS builder

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      build-essential cmake \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY . .
RUN mkdir build -p build && cd build && cmake .. && make


