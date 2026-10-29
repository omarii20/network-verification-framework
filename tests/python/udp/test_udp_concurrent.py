
from concurrent.futures import ThreadPoolExecutor


def send_udp_message(client_id, client, server_address):
    message = f"Hello from Client {client_id}".encode()

    client.sendto(message, server_address)

    response, response_address = client.recvfrom(1024)

    print(f"Client {client_id} received from "f"{response_address}: {response.decode()}")

    return message, response, response_address


def test_udp_concurrent_clients(udp_client_factory, udp_server_address):

    # Create 3 UDP clients using the fixture
    clients = [udp_client_factory() for _ in range(3)]

    # Run the 3 clients concurrently
    with ThreadPoolExecutor(max_workers=3) as executor:
        futures = [
            executor.submit(
                send_udp_message,
                client_id,
                client,
                udp_server_address
            )
            for client_id, client in enumerate(clients, start=1)
        ]

        results = [future.result() for future in futures]

    # Verify each client received its own message
    for message, response, response_address in results:
        assert response == message
        assert response_address == udp_server_address
