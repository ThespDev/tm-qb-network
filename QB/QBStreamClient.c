
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"  // Replace with the actual server IP
#define SERVER_PORT 9002       // Replace with the actual server port

void read_request(int socket, char* buffer) {
    recv(socket, buffer, BUFFER_SIZE, 0);
    buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character
}

void send_response(int socket, const char* response) {
    send(socket, response, strlen(response), 0);
}

int main(int argc, char* argv[]) {
    int socket_desc;
    struct sockaddr_in server_addr;
    char server_ip[] = SERVER_IP;

    // Check the language mode from command line arguments
    char* mode = argv[1];
    int javamode = 0, pythonmode = 0, cmode = 0;

    if (strcmp(mode, "java") == 0)
        javamode = 1;
    else if (strcmp(mode, "c") == 0)
        cmode = 1;
    else if (strcmp(mode, "python") == 0)
        pythonmode = 1;
    else {
        printf("Invalid language mode\n");
        return 1;
    }

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        perror("Failed to create socket");
        return 1;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, server_ip, &(server_addr.sin_addr)) <= 0) {
        perror("Invalid address/ Address not supported");
        return 1;
    }

    // Connect to the server
    if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // QB receives the language mode from TM
    char language_mode[BUFFER_SIZE];
    read_request(socket_desc, language_mode);
    printf("Language mode received: %s\n", language_mode);

    // QB receives a request for random questions from TM
    char request_type[BUFFER_SIZE];
    read_request(socket_desc, request_type);
    printf("Request type received: %s\n", request_type);

    // QB sends a response with random question numbers to TM
    send_response(socket_desc, "RAND_Q\n");
    send_response(socket_desc, "[1, 2, 3, 4]\n");

    // QB receives a request for question content from TM
    read_request(socket_desc, request_type);
    printf("Request type received: %s\n", request_type);

    // QB sends a response with question content to TM
    send_response(socket_desc, "Q_CONTENT\n");
    send_response(socket_desc, "Question content goes here\n");

    if (javamode)
        send_response(socket_desc, "m\njava\nc\npython\nhaskell\n");
    else if (cmode)
        send_response(socket_desc, "c\n");
    else if (pythonmode)
        send_response(socket_desc, "m\n");

    // QB receives a request for marking the response from TM
    read_request(socket_desc, request_type);
    printf("Request type received: %s\n", request_type);

    // QB sends a response with marking results to TM
    send_response(socket_desc, "MARKING\n");
    send_response(socket_desc, "correct\n");

    // Close the socket
    close(socket_desc);

    return 0;