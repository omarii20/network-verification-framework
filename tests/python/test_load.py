from concurrent.futures import ThreadPoolExecutor


CLIENT_COUNT = 100
WORKERS = 20

EXPECTED_RESPONSE = b"Message received successfully\n"


def run_client(client_id, tcp_client_factory):
    client = tcp_client_factory()

    message = f"Load test client {client_id}\n".encode()
    client.sendall(message)

    response = client.recv(1024)

    return response == EXPECTED_RESPONSE


def test_server_load(tcp_client_factory):
    with ThreadPoolExecutor(max_workers=WORKERS) as executor:
        futures = []

        for i in range(CLIENT_COUNT):
            future = executor.submit(
                run_client,
                i,
                tcp_client_factory
            )

            futures.append(future)

        results = [future.result() for future in futures]

    assert all(results)