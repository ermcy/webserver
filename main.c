#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <bits/signum-generic.h>

void handle_signal(int signal) {
    printf("Caught signal %d\n", signal);
}

int main() {
    fflush(stdout);
    signal(SIGKILL, handle_signal);
    char buffer[BUFSIZ];
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("webserver (socket)");
        return 1;
    }
    printf("socket created successfully\n");
    struct sockaddr_in host_addr;
    int hostaddrlen = sizeof(host_addr);
    struct sockaddr_in client_addr;
    int clientaddrlen = sizeof(client_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(6969);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr *)&host_addr, hostaddrlen) != 0) {
        perror("webserver (bind)");
        return 1;
    }
    printf("socket successfully bound to address\n");
    if (listen(sockfd, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        return 1;
    }
    printf("server listening for connections\n");
    for (;;) {
        int newsocketfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&hostaddrlen);
        if (newsocketfd < 0) {
            perror("webserver (accept)");
            continue;
        }
        int sockn = getsockname(newsocketfd, (struct sockaddr *)&client_addr, (socklen_t *)&clientaddrlen);
        if (sockn < 0) {
            perror("webserver (getsockname)");
            continue;
        }
        printf("connection accepted\n");
        ssize_t vale_read = read(newsocketfd, buffer, sizeof(buffer));
        if (vale_read < 0) {
            perror("webserver (read)");
            continue;
        }
        char method[2048], uri[2048], version[2048];
        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("%s\n", buffer);
        printf("[%s:%u]\n %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(clientaddrlen), method, uri, version);
        if (strcmp(method, "GET") == 0) {
            char *resp = "PERFORMED HTTP GET METHOD";
            ssize_t val_write = write(newsocketfd,resp, strlen(resp));
            if (val_write < 0) {
                perror("webserver (write)");
                continue;
            }
        }

        if (strcmp(method, "POST") == 0) {
            char *resp = "PERFORMED HTTP POST METHOD";
            ssize_t val_send = send(newsocketfd, resp, strlen(resp), 0);
            ssize_t val_write = write(newsocketfd,buffer, strlen(buffer));
            if (val_send < 0) {
                perror("webserver (write)");
                continue;
            }
        }
        /*ssize_t val_write = write(newsocketfd, resp, strlen(resp));
        if (val_write < 0) {
            perror("webserver (write)");
            continue;
        }*/
        close(newsocketfd);
    }
    return 0;
}