#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>


#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"  // Replace with the actual server IP
#define SERVER_PORT 9002       // Replace with the actual server port
#define LINELENGTH 5000 
#define READ_END 0  // Read end of the pipe
#define WRITE_END 1  // Write end of the pipe

#define MAX_LINE_LENGTH 1024  // Maximum length of a line in the input file

//structure definitons probably going to have to use in diff program
//TODO optimise for better memory useage

struct multi_choiceq {
  int qnum;
  char *lang;
  char qtext[100];
  char* options[4]; //2d array
  int answer; //1 = a, 2 = b, 3 = c, 4 = d e.t.c. 
};

struct programq {
  int qnum;
  char* qtext;
  char* lang; //C99, Java or Python
  char* inputs[3];  //What is expected to go in
  char* outputs[3]; //What is expected to come out 
};

struct parsedcsv{
  struct programq programqs[100];
  struct multi_choiceq multi_choiceqs[100];
  int numq;
  int cnum;
  int mcanum;
};

//Functions go here
extern struct parsedcsv parsingcsv(char *, char*);
extern void randomQ(int,int,int *);
extern char *exec(char *command,char *);
extern char *delimfinder(char*,char*,char,int);
extern void usage();
extern void randomLoop(int *, int,int,int,int);
