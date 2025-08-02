# build stage
FROM debian:bullseye AS builder
RUN apt-get update && apt-get install -y build-essential cmake
WORKDIR /src
COPY . .
RUN mkdir build && cd build && cmake .. && make

# Runtime stage
FROM debian:bullseye-slim
WORKDIR /app
COPY --from=builder /src/build/udp_client ./
ENTRYPOINT ["./udp_client"]
