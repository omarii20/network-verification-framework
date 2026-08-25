import socket
import pytest
import subprocess
import time

HOST = "127.0.0.1"
PORT = 8080

GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"


def create_client():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.settimeout(3)
    client.connect((HOST, PORT))
    return client


@pytest.fixture
def tcp_client():
    client = create_client()

    yield client

    client.close()


@pytest.fixture
def tcp_client_factory():
    clients = []

    def create():
        client = create_client()
        clients.append(client)
        return client

    yield create

    for client in clients:
        client.close()


@pytest.fixture(scope="session", autouse=True)
def server_process():

    # Clean the server build before starting the process
    clean_result = subprocess.run(["make", "clean"], capture_output=True, text=True)
    if clean_result.returncode != 0:
            pytest.fail(
                f"Server clean failed:\n{clean_result.stderr}"
            )

    # Build the server before starting the process
    build_result = subprocess.run(["make"], capture_output=True, text=True)

    if build_result.returncode != 0:
        pytest.fail(
            f"Server build failed:\n{build_result.stderr}"
        )

    if is_port_in_use(HOST, PORT):
        pytest.fail(
            f"{RED}Port {PORT} is already in use. "
            f"Stop the existing server before running tests.{RESET}"
        )

    # Start the server process
    process = subprocess.Popen(["./server"])

    wait_for_server(process, HOST, PORT)

    yield process

    process.terminate()
    process.wait()

# Server Readiness Check
def wait_for_server(process, host, port, timeout=5):
    start_time = time.time()

    while time.time() - start_time < timeout:

        # Check that the server process we started is still alive
        if process.poll() is not None:
            pytest.fail(
                f"{RED}Server process exited unexpectedly "
                f"with code {process.returncode}{RESET}"
            )
        try:
            with socket.create_connection((host, port), timeout=0.2):
                print(
                    f"{GREEN}Server is ready on "
                    f"{host}:{port}{RESET}"
                )
                return

        except OSError:
            time.sleep(0.1)

    pytest.fail(
        f"{RED}Server did not become ready on "
        f"{host}:{port} within {timeout} seconds{RESET}"
    )

def is_port_in_use(host, port):
    try:
        with socket.create_connection((host, port), timeout=0.2):
            return True
    except OSError:
        return False