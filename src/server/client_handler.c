
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#include "client_handler.h"
#include "server_config.h"

/*
 * מטפלת באירוע קריאה אחד של Client.
 *
 * מחזירה:
 *  0  -> ה-Client עדיין מחובר
 * -1  -> צריך לסגור ולהסיר את ה-Client
 */

 int handle_client_event(int client_fd){
    char buffer[BUFFER_SIZE];

    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received == -1) {
        /*
         * Non-blocking socket:
         * there is currently no more data available to read.
        */
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }

        perror("recv");
        return -1;
    }

    /*
     * recv() returned 0:
     * the client closed the connection.
     */
    if (bytes_received == 0) {
        printf("Client disconnected, FD %d\n", client_fd);
        return -1;
    }

    buffer[bytes_received] = '\0';
    printf("Received %zd bytes from FD %d\n",bytes_received, client_fd);
    printf("Message from FD %d: %s", client_fd, buffer);

    /*
     * Prepare server response.
     */
    const char *response ="Message received successfully\n";
    size_t response_length = strlen(response);
    ssize_t bytes_sent = send(client_fd, response, response_length, 0);

    if (bytes_sent == -1) {
        //The socket is currently not ready for writing.
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("FD %d is not ready for writing yet\n", client_fd);
            return 0;
        }

        perror("send");
        return -1;
    }

    /*
     * On a non-blocking socket send() may send
     * fewer bytes than requested.
     */
    if ((size_t)bytes_sent < response_length) {
        printf("Partial send on FD %d: sent %zd of %zu bytes\n", client_fd, bytes_sent, response_length);
    }

    printf("Sent %zd bytes to client FD %d\n", bytes_sent,client_fd);

    return 0;
}
