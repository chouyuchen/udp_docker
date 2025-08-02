# build stage
FROM debian:bullseye AS builder
RUN apt-get update && apt-get install -y build-essential cmake
WORKDIR /src
COPY . .                         
RUN mkdir build && cd build && cmake .. && make

# Runtime stage
FROM debian:bullseye-slim
WORKDIR /app
COPY --from=builder /src/build/udp_server ./udp_server
EXPOSE 9000/udp
ENTRYPOINT ["./udp_server"]
