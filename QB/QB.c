#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 6000
#define SA struct sockaddr

void func(int sock_fd)
{
    char buff[MAX];
    int n;
    for (;;) {
        // bzero() zero out memory
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sock_fd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sock_fd, buff, sizeof(buff));
        printf("From Test Manager : %s", buff);
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Question Bank Exit...\n");
            break;
        }
    }
}
 
int main()
{
    int sock_fd, conn_fd;
    struct sockaddr_in servaddr, cli;
 
    // Socket creation and verify
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        printf("Socket has failed to be created...\n");
        exit(0);
    }
    else
        printf("Socket has successfully been created..\n");
    bzero(&servaddr, sizeof(servaddr));
 
    // Assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
 
    // Connect the client socket to server socket
    if (connect(sock_fd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("Failed to connect to Test Manager...\n");
        exit(0);
    }
    else
        printf("Successfully connected to Test Manager..\n");
 
    // Function for chat
    func(sock_fd);
 
    // Close the socket
    close(sock_fd);
}

