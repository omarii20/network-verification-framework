# Network Verification Framework

A Linux-based network verification and performance testing framework developed
using C, POSIX sockets, TCP, UDP, I/O multiplexing, multithreading, and Python.

## Project Goals

- Validate TCP and UDP communication.
- Support multiple concurrent client connections.
- Explore different server concurrency models:
  - Thread-per-client (POSIX Threads)
  - I/O multiplexing with `poll()`
  - `epoll()` (planned)
- Run automated functional and stress tests.
- Measure latency, throughput, and packet loss.
- Simulate connection failures, timeouts, and reconnections.
- Produce structured logs and test reports.

## Technology Stack

- C11
- POSIX Sockets
- POSIX Threads
- TCP and UDP
- I/O Multiplexing (`poll`)
- Linux
- Python 3
- Make
- GitHub Actions

## Current Progress

### ✅ Completed

- TCP server implementation.
- TCP client implementation.
- Socket creation (`socket()`).
- Address binding (`bind()`).
- Listening socket (`listen()`).
- Client connection handling (`accept()`).
- Data transmission using `send()` and `recv()`.
- Thread-per-client server implementation using POSIX Threads.
- Event-driven server implementation using `poll()`.
- Dynamic client management with `struct pollfd`.
- Connection cleanup and client removal.
- Handling disconnects and socket errors (`POLLHUP`, `POLLERR`, `POLLNVAL`).

### 🚧 In Progress

- Learning and optimizing I/O multiplexing.
- `epoll()` implementation.
- Code refactoring and modularization.

### 📌 Planned

- UDP implementation.
- Performance benchmarking.
- Stress testing with hundreds/thousands of concurrent clients.
- Structured logging.
- Python automation tools.
- CI/CD integration with GitHub Actions.

## Status

The project is under active development.
Current focus: mastering Linux network programming and scalable server architectures.