
def test_udp_echo(udp_client, udp_server_address):
    message = b"Hello UDP Server"

    udp_client.sendto(message, udp_server_address)

    response, server_address = udp_client.recvfrom(1024)

    print(f"Received: {response.decode()}")

    assert response == message
    assert server_address == udp_server_address
