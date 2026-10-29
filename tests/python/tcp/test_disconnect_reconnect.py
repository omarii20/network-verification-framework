def test_disconnect_reconnect(tcp_client_factory):
    client1 = tcp_client_factory()

    client1.sendall(b"First client\n")
    response1 = client1.recv(1024)

    assert response1 == b"Message received successfully\n"

    client1.close()

    client2 = tcp_client_factory()

    client2.sendall(b"Second client\n")
    response2 = client2.recv(1024)

    assert response2 == b"Message received successfully\n"