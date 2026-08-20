import socket
import pytest

HOST = "127.0.0.1"
PORT = 8080


@pytest.fixture
def tcp_client():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((HOST, PORT))

    yield client

    client.close()