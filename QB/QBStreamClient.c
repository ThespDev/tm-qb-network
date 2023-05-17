#include "qb.h"

void read_request(int socket, char* buffer) {
    recv(socket, buffer, BUFFER_SIZE, 0);
    buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character
}

void send_response(int socket, const char* response) {
    send(socket, response, strlen(response), 0);
}

        //Requires 3 Arguments - IP of server, Language used and CSV file location
//"Usage: ./QB <TM-IP-Address> <CSV-File-location> <Language #(C,Java or Python)>")
int main(int argc, char* argv[]) {

    if(argc != 4){
      printf("Error invalid amount of arguments\n Argument count: %i\n",argc);
      usage();
    } 
    
    int socket_desc;
    struct sockaddr_in server_addr;
    char *server_ip = strdup(argv[1]);
    printf("Server IP now spinning up on IP of %s \n",server_ip);
    char *CSVFile = strdup(argv[2]);
    char *mode = strdup(argv[3]);

    // Check the language mode from command line arguments
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
   
    printf("\nServer running off CSV File %s \nWill do answers in Language %s\n",CSVFile,mode);
    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Failed to create socket\n");
        printf("Languages are: c, java, or pythong");
        printf("\n Also have to be in lower case for now");
        usage();
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

    //QB recives inital ACK from server
    printf("\nConnection made, awaiting for inital ACK");
    char ack[BUFFER_SIZE];
    read_request(socket_desc, ack);
    if ( strcmp(ack,"ack:TMserver")){
      perror("Error: Proper TM ack not recived, exiting");
      usage();
    }
    printf("ACK recived\n");
    
    // QB Sends the language mode to TM
    printf("Sending Language '%s' info to specified server\n",mode);
    send_response(socket_desc, mode);

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
}
