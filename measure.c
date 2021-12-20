#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#define SIZE 1260785
#define BILLION 1000000000.0

int fsize(FILE *fp) {
    int prev = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fseek(fp, prev, SEEK_SET); //go back to where we were
    return sz;
}

void write_file(int sockf) {
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    int n = 0;
    int num = 0;
    char buffer[SIZE];
    FILE *fp;
    int tmp;
    char *filename = "bigfile.txt";
    fp = fopen(filename, "r");
    int size = fsize(fp);
    int count = 0;
    int bool = 1;
    int sum = 0;
    while (sum <= 10*SIZE) {
    	//printf("size: %d\n", size);
        //tmp = n;
        n = recv(sockf, buffer, SIZE, 0);
        //printf("%d ", n);
        if (n <= 0) {
            break;
            return;
        }
        sum = sum + strlen(buffer);
        num = num + strlen(buffer);
        if (sum >= 5*SIZE && bool) {
            bool =0 ;
            clock_gettime(CLOCK_REALTIME, &end);
            double time_spent = (end.tv_sec - start.tv_sec) +
                                (end.tv_nsec - start.tv_nsec) / BILLION;
            printf("Time for transfer by cubic is: %f \n", time_spent);
            clock_gettime(CLOCK_REALTIME, &start);
            printf("The sum is: %d ", sum);
        }
        bzero(buffer, SIZE);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    double time_spent = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / BILLION;
    printf("Time for transfer by reno is: %f \n", time_spent);
    //printf("%d", num);
    printf("The sum is: %d\n", sum);
    return;
}

int main() {
    char *ip = "127.0.0.1";
    int port = 10000;
    int sockf, new_sock;
    struct sockaddr_in server_addr, new_addr;
    socklen_t addr_size;
    char buffer[SIZE];

    sockf = socket(AF_INET, SOCK_STREAM, 0);
    if (sockf < 0) {
        perror("socket error");
        exit(1);
    }
    printf("Server socket created successfully.\n");
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    int err = bind(sockf, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (err < 0) {
        perror("bind error");
        exit(1);
    }
    printf("Binding successfully.\n");
    if (listen(sockf, 10) == 0) {
        printf("Connected...\n");
    } else {
        perror("Error in listening");
        exit(1);
    }
    addr_size = sizeof(new_addr);
    new_sock = accept(sockf, (struct sockaddr *) &new_addr, &addr_size);
    write_file(new_sock);
    printf("The file was received successfully");
    return 0;
}

