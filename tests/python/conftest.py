import socket
import pytest

HOST = "127.0.0.1"
PORT = 8080


def create_client():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
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