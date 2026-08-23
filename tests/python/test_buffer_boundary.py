def test_buffer_boundary(tcp_client):
    message = b"A" * 2048

    tcp_client.sendall(message)

    response = tcp_client.recv(1024)

    assert response == b"Message received successfully\n"