
import socket
import subprocess
import time

import pytest

HOST = "127.0.0.1"
UDP_PORT = 9090

def wait_for_udp_server(process, timeout=5):
    start_time = time.monotonic()

    while time.monotonic() - start_time < timeout:
        if process.poll() is not None:
            pytest.fail(f"UDP server exited with code {process.returncode}")

        try:
            with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as client:
                client.settimeout(0.2)
                client.sendto(b"READY_CHECK", (HOST, UDP_PORT))

                data, _ = client.recvfrom(1024)

                if data == b"READY_CHECK":
                    return
        except socket.timeout:
            pass
        except OSError:
            pass

        time.sleep(0.1)

    pytest.fail("UDP server did not become ready within 5 seconds")

@pytest.fixture
def udp_client():
    client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client.settimeout(3)

    yield client

    client.close()

@pytest.fixture
def udp_server_address():
    return (HOST, UDP_PORT)

@pytest.fixture
def udp_client_factory():
    clients = []

    def create():
        client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        client.settimeout(3)
        clients.append(client)
        return client

    yield create

    for client in clients:
        client.close()

@pytest.fixture(scope="session", autouse=True)
def udp_server_process():
    build = subprocess.run(["make", "udp_server"], capture_output=True, text=True)

    if build.returncode != 0:
        pytest.fail(f"UDP server build failed:\n{build.stderr}")

    process = subprocess.Popen(["./udp_server"])

    try:
        wait_for_udp_server(process)
        yield process
    finally:
        if process.poll() is None:
            process.terminate()
            try:
                process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
