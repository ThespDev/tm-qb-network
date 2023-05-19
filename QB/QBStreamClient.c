#include "qb.h"
#include <stdio.h>


void read_request(int socket, char* buffer) {
    recv(socket, buffer, BUFFER_SIZE, 0);
    // Remove newline character
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
    
    struct parsedcsv CSVFIlePars = parsingcsv(CSVFile,mode);
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
    printf("\nConnection made, awaiting for inital ACK\n");
   // char ack[BUFFER_SIZE];
   // read_request(socket_desc, ack);
   // if ( strcmp(ack,"ack:TMserver")){
   //   perror("Error: Proper TM ack not recived, exiting");
   //   usage();
   // }
    printf("ACK recived\n");
    
    // QB Sends the language mode to TM
    printf("Sending Language '%s' info to specified server\n",mode);
    send_response(socket_desc, mode);

 ////PRIMARY LOOP, responses and returns----------------
    printf("Inital Setup done, Awaiting contanct from server...\n");
    char *Requesttype = calloc(BUFFER_SIZE,sizeof(char));
    char *messageID = calloc(4,sizeof(char));
    while (1){//Looping server mode 
      strcpy(Requesttype,"");
      read_request(socket_desc,Requesttype);      
      char responsetext[BUFFER_SIZE*2];
      strcpy(messageID,"");
      strcat(messageID,&Requesttype[strlen(Requesttype)-2]);
      //Get last two charachters of a string, convert it to a number
      int response = 0;
      Requesttype[strlen(Requesttype)-3]='\0';

    
      
      //RANDOM Q REQUEST
      //If server requests a random question (maybe turn into function later)
      if (strncmp(Requesttype,"RAND_Q",6)==0){
        strcpy(responsetext,"RAND_Q\n");
        char Type[3];
        strcpy(Type,Requesttype+7); //Cut off rest of text wit how small
        char num[2];
        strcpy(num,Requesttype+10);
        int amountofq;
        amountofq = atoi(num); 
        int randqlist[amountofq];
        if (strcmp(Type,"MCA")==0)
          randomQ(amountofq, CSVFIlePars.mcanum,randqlist);
        else
          randomQ(amountofq, CSVFIlePars.cnum,randqlist);
        strcat(responsetext,"[");
        int i = 0;
        for (; i < amountofq; i++){
          char temp[4];
          sprintf(temp,"%i",randqlist[i]);
          strcat(responsetext,temp);
          strcat(responsetext,",");
        }
        responsetext[strlen(responsetext)-1] = ']';
        response = 1;  
      }


      //CONTENT OF Q REQUEST
      //FOR WHEN SERVER REQUESTS CONTENT OF QUESTION
      else if(strncmp(Requesttype,"Q_CONTENT",8)==0){ 
        strcpy(responsetext,"Q_CONTENT\n");
        char Type[4];
        strncpy(Type,Requesttype+10,3);
        Type[3] = '\0';
        char num[2];
        strncpy(num,Requesttype+14,2);
        int qnum;
        qnum = atoi(num);
        if (strcmp(Type,"MCA")==0){
          strcat(responsetext,"m\n");
          strcat(responsetext,CSVFIlePars.multi_choiceqs[qnum].qtext);
          strcat(responsetext,"\n");
          for (int x =0; x < 4; x++){
            strcat(responsetext,CSVFIlePars.multi_choiceqs[qnum].options[x]);
            strcat(responsetext,"\n");
          }
          responsetext[strlen(responsetext)-1] = '\0';
        }
        else {
          strcat(responsetext,"c\n");
          strcat(responsetext,CSVFIlePars.programqs[qnum].qtext); 
        }
        response = 1;
      }
      
      //MARKING REQUESTS
      //FOR WHEN WE WANT MARKS
      //One must be wary of the specter of
      // Karl Marks ☭
      else if (strncmp(Requesttype,"MARKING",6)==0){
        strcpy(responsetext,"MARKING\n");
        char Type[4];
        strncpy(Type,Requesttype+8,3);
        Type[3] = '\0';
        char num[2];
        strncpy(num,Requesttype+12,2);
        int qnum;
        qnum = atoi(num);
        strncat(responsetext,num,2);
        strcat(responsetext,"\n");
        strncat(responsetext,Type,3);
        char StAnswer[strlen(Requesttype)]; //member of the holy c
        strcpy(StAnswer,Requesttype+15);
        if (strcmp(Type,"MCA")==0){
          puts(StAnswer);
          if(CSVFIlePars.multi_choiceqs[qnum].answer==atoi(StAnswer))
            strcat(responsetext,"\nCorrect");
          else 
            strcat(responsetext,"\nIncorrect");
        }

        else{ //Handling code it self mine gott
          FILE * Codefle;
          char runcommand[50];
          char command[BUFFER_SIZE];
          char Output[BUFFER_SIZE];
          if (javamode){
            FILE * Codefle = fopen("./StdntAnswer.java","w"); 
            if (Codefle == NULL){printf("\nError, Couldn't setup Java file\n"); usage();}
            strcpy(command,"javac StdntAnswer.java");
            strcpy(runcommand,"java StdntAnswer");
            fprintf(Codefle,"%s\n",StAnswer);
            fclose(Codefle);
          }
          else if (cmode){
            FILE * Codefle = fopen("./StdntAnswer","w"); if (Codefle == NULL){printf("Error, Couldn't setup C file\n"); usage();} 

            strcpy(command,"gcc -o test -x c ./StdntAnswer");
            strcpy(runcommand,"./test");
            fprintf(Codefle,"%s\n",StAnswer);
            fclose(Codefle);
          }
          int ReturedV = system(command);
          if (ReturedV != 0 ){
              printf("Compile Error\n");
              strcat(responsetext,"\nIncorrect");
          }

          else{
            char Cattext[20];
            strncpy(Cattext,"\nCorrect\0",9);
            for(int x = 0; x < 3; x++){
                strcpy(Output,"");
                if ((CSVFIlePars.programqs[qnum].inputs[x])!=NULL)
                  strcat(runcommand,CSVFIlePars.programqs[qnum].inputs[x]);
                exec(runcommand,Output);
                char extra[3] = "";
                if (javamode){strcpy(extra,"\n\0");} 
                char *expectedanswer = CSVFIlePars.programqs[qnum].outputs[x];
                strcat(expectedanswer,extra);
                if (strcmp(Output,expectedanswer)!=0){
                  puts(CSVFIlePars.programqs[qnum].outputs[x]);
                  printf("\nResponses do not line up to expected values\n");
                  strncpy(Cattext,"\nIncorrect",10);
                  break;
                }
              }
            strcat(responsetext,Cattext);
            }
          //Code Generated and put in fild, now time to execute 
      }
      response =1;
     }


    if (response){
      strcat(responsetext,"\n");
      strcat(responsetext,messageID);
      send_response(socket_desc, responsetext);
      printf("Data Recived and Response sent to TM Server\n");
      response = 0;
      }
    }
       

//    // QB receives a request for random questions from TM
//    char request_type[BUFFER_SIZE];
//    read_request(socket_desc, request_type);
//    printf("Request type received: %s\n", request_type);
//
//
//    // QB receives a request for question content from TM
//    read_request(socket_desc, request_type);
//    printf("Request type received: %s\n", request_type);
//
//    // QB sends a response with question content to TM
//    send_response(socket_desc, "Q_CONTENT\n");
//    send_response(socket_desc, "Question content goes here\n");
//
//    if (javamode)
//        send_response(socket_desc, "m\njava\nc\npython\nhaskell\n");
//    else if (cmode)
//        send_response(socket_desc, "c\n");
//    else if (pythonmode)
//        send_response(socket_desc, "m\n");
//
//    // QB receives a request for marking the response from TM
//    read_request(socket_desc, request_type);
//    printf("Request type received: %s\n", request_type);
//
//    // QB sends a response with marking results to TM
//    send_response(socket_desc, "MARKING\n");
//    send_response(socket_desc, "correct\n");
//
//    // Close the socket
    close(socket_desc);

    return 0;
}
