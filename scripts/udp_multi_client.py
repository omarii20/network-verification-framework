import socket
import threading


def udp_client(client_id):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.settimeout(3)

        message = f"Hello from Client {client_id}"

        sock.sendto(
            message.encode(),
            ("127.0.0.1", 9090)
        )

        data, addr = sock.recvfrom(1024)

        print(
            f"Client {client_id} received "
            f"from {addr}: {data.decode()}"
        )


threads = []

for i in range(1, 4):
    thread = threading.Thread(
        target=udp_client,
        args=(i,)
    )

    threads.append(thread)
    thread.start()

for thread in threads:
    thread.join()