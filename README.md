# Network Verification Framework

A Linux-based network verification and performance testing framework developed
using C, POSIX sockets, TCP, UDP, I/O multiplexing, multithreading, and Python.

## Project Goals

* Validate TCP and UDP communication.
* Support multiple concurrent client connections.
* Explore different server concurrency models:

  * Thread-per-client using POSIX Threads.
  * I/O multiplexing with `poll()`.
  * Linux event-driven I/O using `epoll()`.
* Run automated functional and stress tests.
* Measure latency, throughput, and packet loss.
* Simulate connection failures, timeouts, and reconnections.
* Produce structured logs and test reports.

## Technology Stack

* C11
* POSIX Sockets
* POSIX Threads
* TCP and UDP
* I/O Multiplexing (`poll`)
* Linux `epoll`
* Linux
* Python 3
* Make
* GitHub Actions

## Current Progress

### ✅ Completed

* TCP server implementation.
* TCP client implementation.
* Socket creation (`socket()`).
* Address binding (`bind()`).
* Listening socket (`listen()`).
* Client connection handling (`accept()`).
* Data transmission using `send()` and `recv()`.
* Thread-per-client server implementation using POSIX Threads.
* Event-driven server implementation using `poll()`.
* Dynamic client management with `struct pollfd`.
* Linux event-driven server implementation using `epoll()`.
* `epoll` instance creation using `epoll_create1()`.
* File descriptor registration using `epoll_ctl()`.
* Event waiting and dispatch using `epoll_wait()`.
* Dynamic client registration and removal from `epoll`.
* Handling client disconnects and socket errors.
* Multiple concurrent client connections without one thread per client.

### 🚧 In Progress

* Understanding and reviewing the complete `epoll` event flow.
* Improving scalable I/O handling.
* Exploring non-blocking sockets.
* Exploring edge-triggered `epoll` (`EPOLLET`).

### 📌 Planned

* Code refactoring and modularization.
* Python-based automated functional tests.
* Performance benchmarking.
* Stress testing with hundreds/thousands of concurrent clients.
* UDP implementation.
* Structured logging.
* CI/CD integration with GitHub Actions.

## Status

The project is under active development.

Current focus: understanding Linux `epoll`, event-driven networking, and scalable server architecture before moving to non-blocking I/O, refactoring, automation, and UDP.
