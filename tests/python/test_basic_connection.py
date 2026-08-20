def test_basic_connection(tcp_client):
    tcp_client.sendall(b"Hello from pytest222\n")

    response = tcp_client.recv(1024)
    print("Response from server:", response)

    assert response == b"Message received successfully\n"