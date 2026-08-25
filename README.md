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
* Non-blocking I/O
* Linux
* Python 3
* pytest
* Make
* GitHub Actions

## Current Progress

### ✅ Completed

#### TCP Networking

* TCP server implementation.
* TCP client implementation.
* Socket creation using `socket()`.
* Address binding using `bind()`.
* Listening socket using `listen()`.
* Client connection handling using `accept()`.
* Data transmission using `send()` and `recv()`.
* Thread-per-client server implementation using POSIX Threads.
* Event-driven server implementation using `poll()`.
* Dynamic client management using `struct pollfd`.
* Linux event-driven server implementation using `epoll()`.
* `epoll` instance creation using `epoll_create1()`.
* File descriptor registration using `epoll_ctl()`.
* Event waiting and dispatch using `epoll_wait()`.
* Dynamic client registration and removal from `epoll`.
* Handling client disconnects and socket errors.
* Multiple concurrent client connections without one thread per client.
* Non-blocking listening and client sockets.
* Handling `EAGAIN` and `EWOULDBLOCK`.

#### Code Architecture

* Server code refactored into separate modules:
  * `server.c`
  * `socket_utils.c/.h`
  * `client_handler.c/.h`
  * `epoll_manager.c/.h`
  * `server_config.h`
* Makefile for server build, execution, cleanup, and test execution.

#### Python Test Automation

* pytest-based TCP automation framework.
* Automatic server build before test execution.
* Automatic server startup and shutdown.
* Server readiness check before running tests.
* Detection of an already occupied server port.
* Socket timeouts to prevent hanging tests.
* Reusable TCP client fixtures.
* TCP client factory fixture for multi-client tests.

#### Automated TCP Tests

* Basic TCP connection and response validation.
* Multiple messages over the same TCP connection.
* Client disconnect and reconnect validation.
* Multiple concurrent client connections.
* TCP payload handling beyond the receive buffer size.
* Concurrent load test using `ThreadPoolExecutor`.
* Load validation with 100 clients and multiple worker threads.

### 🚧 In Progress

* UDP networking implementation.
* Expanding network verification coverage.

### 📌 Planned

* UDP server implementation.
* UDP client implementation.
* Automated UDP functional tests.
* UDP packet loss and timeout testing.
* Performance benchmarking.
* Latency and throughput measurements.
* Larger-scale stress testing.
* Structured logging and test reports.
* CI/CD integration with GitHub Actions.
* Additional scalable I/O experiments where relevant.

## Running the Project

Build the TCP server:

```bash
make