def test_multiple_messages(tcp_client):
    messages = [
        b"First message\n",
        b"Second message\n",
        b"Third message\n",
    ]

    expected_response = b"Message received successfully\n"

    for message in messages:
        tcp_client.sendall(message)

        response = tcp_client.recv(1024)

        assert response == expected_response