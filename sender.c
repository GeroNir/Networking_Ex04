#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#define SIZE 1260785

void send_file(FILE *file, int sockf) {
    int num = 0;
    char data[SIZE] = {0};
    while (fgets(data, SIZE, file) != NULL) {
        if (send(sockf, data, SIZE, 0) == -1) {
            perror("sending file error");
            exit(1);
        }
        num = num + strlen(data);
        bzero(data, SIZE);
    }
    printf("%d bytes were sent", num);
}

int main(int argc, char **argv) {
    char buf[256];
    socklen_t len;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return -1;
    }

    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
        perror("getsockopt");
        return -1;
    }

    printf("Current: %s\n", buf);
    char *ip = "127.0.0.1";
    int port = 10000;
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    int err = connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err == -1) {
        perror("socket error");
        exit(1);
    }
    printf("Connected to Server.\n");
    char *filename = "bigfile.txt";
    for (int i = 0; i < 5; i++) {
        FILE *file;
        file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error in reading file.");
            exit(1);
        }
        send_file(file, sock);
        fclose(file);
        printf("File data sent successfully.\n");
    }

    strcpy(buf, "reno");
    len = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
        perror("setsockopt");
        return -1;
    }
    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
        perror("getsockopt");
        return -1;
    }
    printf("New: %s\n", buf);
    for (int i = 0; i < 5; i++) {
        FILE *file;
        file = fopen(filename, "r");
        if (file == NULL) {
            perror("reding file error");
            exit(1);
        }
        send_file(file, sock);
        fclose(file);
        printf("File data sent successfully.\n");
    }
    printf("closing connection");

    close(sock);
    return 0;
}

