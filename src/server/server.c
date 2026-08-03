#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <poll.h>
#include <errno.h>

#define PORT 8080
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

/*
 * מטפלת באירוע קריאה אחד של Client.
 *
 * מחזירה:
 *  0  -> ה-Client עדיין מחובר
 * -1  -> צריך לסגור ולהסיר את ה-Client
 */

int handle_client_event(int client_fd){
    
    char buffer[BUFFER_SIZE];

    ssize_t bytes_received = recv( client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received == -1){
        perror("recv");
        return -1;
    }

    if (bytes_received == 0){
        printf("Client disconnected, FD %d\n", client_fd);
        return -1;
    }

    buffer[bytes_received] = '\0';

    printf("Received %zd bytes from FD %d\n", bytes_received, client_fd);
    printf("Message from FD %d: %s", client_fd, buffer);

    const char *response = "Message received successfully\n";
    size_t response_length = strlen(response);

    ssize_t bytes_sent = send( client_fd, response, response_length, 0);

    if (bytes_sent == -1){
        perror("send");
        return -1;
    }

    printf("Sent %zd bytes to client FD %d\n", bytes_sent, client_fd);

    return 0;
}

/*
 * מוסיפה Client חדש למערך של poll.
 *
 * מחזירה:
 *  0  -> הצלחה
 * -1  -> אין מקום במערך
 */

 int add_client(struct pollfd poll_fds[], nfds_t *nfds, int client_fd){
    if (*nfds >= MAX_CLIENTS + 1){
        fprintf(stderr, "Max clients reached, cannot add new client FD %d\n", client_fd);
        return -1;
    }

    poll_fds[*nfds].fd = client_fd;
    poll_fds[*nfds].events = POLLIN;
    poll_fds[*nfds].revents = 0;
    (*nfds)++;

    printf("Added client FD %d to poll array, total clients: %zu\n", client_fd, (size_t)(*nfds - 1));

    return 0;
 }

 /*
 * סוגרת ומסירה Client ממערך poll.
 *
 * אנחנו מעבירים את האיבר האחרון למקום של האיבר שנמחק.
 */

 void remove_client(struct pollfd poll_fds[], nfds_t *nfds, nfds_t index){
    int client_fd = poll_fds[index].fd;
    close(client_fd);
    printf("Closed client FD %d\n", client_fd);
    poll_fds[index] = poll_fds[*nfds - 1];
    (*nfds)--;
 }

int main(void){

    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1){
        perror("socket");
        return 1;
    }

    printf("Socket created successfully, FD %d\n", sockfd);

    // Set socket options to allow address reuse
    int option = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1){
        perror("setsockopt");
        close(sockfd);
        return 1;
    }

    // Bind the socket to a specific port (8080)
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // חיבור ה-socket לפורט.
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("bind");
        close(sockfd);
        return 1;
    }

    printf("Socket bound successfully to port %d\n", PORT);

    // Listen for incoming connections
    if (listen(sockfd, 5) == -1){
        perror("listen");
        close(sockfd);
        return 1;
    }

    printf("Server is listening on port %d\n", PORT);

    /*
     * מערך ה-poll.
     * מקום אחד ל-listening socket
     * ועוד MAX_CLIENTS מקומות ל-Clients.
    */
    struct pollfd poll_fds[MAX_CLIENTS + 1];

    /*
     * בהתחלה יש FD תקף אחד בלבד:
     * ה-listening socket.
    */
    nfds_t nfds = 1;

    poll_fds[0].fd = sockfd;
    poll_fds[0].events = POLLIN;
    poll_fds[0].revents = 0;

    /*
     * זוהי הלולאה הראשית של השרת.
     * אין יותר Thread לכל Client.
    */

   while (1) {
        printf("Waiting for events...\n");

        int ready_count = poll(poll_fds, nfds, -1);

        if (ready_count == -1) {
            if (errno == EINTR) {
                continue;
            }

            perror("poll");
            break;
        }

        /*
         * בודקים את ה-listening socket.
         *
         * אם יש עליו POLLIN, יש Client חדש שמחכה ל-accept.
         */
        if (poll_fds[0].revents & POLLIN) {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);

            int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

            if (client_fd == -1) {
                perror("accept");
            } else {
                printf("Client connected successfully, FD %d\n", client_fd);

                if (add_client(poll_fds, &nfds, client_fd) == -1){
                    close(client_fd);
                }
            }

            ready_count--;
        }

        /*
         * מעבר על כל ה-Clients.
         *
         * מתחילים מ-1 כי index 0 הוא listening socket.
         */
        for (nfds_t i = 1; i < nfds && ready_count > 0; i++) {
            short revents = poll_fds[i].revents;

            if (revents == 0) {
                continue;
            }

            ready_count--;

            /*
             * אם יש מידע לקריאה.
             */
            if (revents & POLLIN) {
                int result = handle_client_event( poll_fds[i].fd);

                if (result == -1) {
                    remove_client(poll_fds, &nfds, i);

                    /*
                     * העברנו את האיבר האחרון למקום i.
                     * צריך לבדוק שוב את אותו index.
                     */
                    i--;
                    continue;
                }
            }

            /*
             * טיפול בניתוק או בשגיאה.
             */
            if (revents & (POLLHUP | POLLERR | POLLNVAL)) {
                printf("Socket event on FD %d, revents=%d\n", poll_fds[i].fd, revents);
                remove_client(poll_fds, &nfds, i);
                i--;
            }
        }
    }

    /*
     * סגירת כל ה-FDs לפני סיום השרת.
     */
    for (nfds_t i = 0; i < nfds; i++) {
        close(poll_fds[i].fd);
    }

    return EXIT_SUCCESS;
}