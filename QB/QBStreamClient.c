#include <stdio.h> // standard input and output library
#include <stdlib.h> //standard library
#include <unistd.h> // standard symbolic constants and types
#include <string.h> // string manipulation functions
#include <sys/socket.h> // socket libary
#include <arpa/inet.h> // definition for internet operations

#define MAX 80 // maximum chararters in a message
#define PORT 9002 //port number to connect to
#define SA struct sockaddr // generic socket address structure

void func(int sockfd) //function to handle connection
{
    char buff[MAX]; //buffer to store messages
    int n;
    int q; // number of questions

    for (;;) { // loop until user enters "exit"
        bzero(buff, sizeof(buff)); // zero out memory
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n') // read message from user
            ;
        write(sockfd, buff, sizeof(buff)); // send message to server
        bzero(buff, sizeof(buff)); // zero out memory
        read(sockfd, buff, sizeof(buff)); // recieve message from server
        printf("From Test Manager : %s", buff); // print the recieved message
        if ((strncmp(buff, "exit", 4)) == 0) { // check if user wants to exit
            printf("Question Bank Exit...\n");
            break;
        }
    }
}
 
int main(int argc, char *argv[])
{

    javamode = false;
    pythonmode = false;
    cmode = false;
    char* mode = argv[2];
    if strcmp(mode,"java"):
        javamode = true;
    else if strcmp(mode,"c"):
        cmode = true;
    else if strcmp(mode,"python"):
        pythonmode = true;

    if (argc == RAND_Q\n)
        //The Client needs to get QBFuntions to retrieve random question




    int sockfd, connfd; // socket file descriptors for client and server
    struct sockaddr_in servaddr, cli; // server and client socket address structure
 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // create a socket
    if (sockfd == -1) { // check if socket creation failed
        printf("Socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket has successfully been created...\n"); // print success message
    bzero(&servaddr, sizeof(servaddr)); // zero out memory for server address structure
 
    // Assign IP, PORT
    servaddr.sin_family = AF_INET; // set address family to internet
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // set IP address to localhost
    servaddr.sin_port = htons(PORT); // set port number to PORT
 
    // Connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("Failed to connect to Test Manager...\n");
        exit(0);
    }
    else
        printf("Successfully connected to Test Manager..\n"); // print success message
 
    func(sockfd); // call function to handle connection
 
    close(sockfd); // close the socket
}

