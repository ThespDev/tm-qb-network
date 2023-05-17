// USAGE: write_question_and_answer <filepath> <languageused>

#define LINELENGTH 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
};

const char Usage_msg[] = ("USAGE: ./Write_Questions <file path> <Langauge used>");

void usage(){
  printf("\n%s\n",Usage_msg);
  exit(EXIT_FAILURE);
}


char* getfield(char* line, int num)
{
    char* tok;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}
                                //Either C99, Java or Python
//Function for parsing csv files into strucutres
struct parsedcsv parsingcsv(char *filename,char* Language){
  FILE *fp;

  fp = fopen(filename, "r"); // open file for reading
   if (fp == NULL){
    printf("Internal Error: Unable to open file %s",filename); usage(); 
   }
  
  char line[LINELENGTH];
  
  struct multi_choiceq multiq[100];
  struct programq codeq[100];
  int MCA_Counter = 0;
  int Code_Counter = 0;
  while (fgets(line, LINELENGTH, fp))
    {
        char* tmp = strdup(line);

        //skip commented lines (lines start with hashes)
        if (tmp[0] == '#'){
          continue;
        }
        char *token = strtok(tmp,",");
        //TODO optimise for better memory useage
        if (strcmp(token,"MCA") == 0 ){
          multiq[MCA_Counter].lang = (char*)calloc(strlen(Language),sizeof(char));
            memcpy(multiq[MCA_Counter].lang,Language,strlen(Language));
          token = strtok(NULL,",");
          multiq[MCA_Counter].qnum = atoi(token);
          token = strtok(NULL,",");
          strcpy(multiq[MCA_Counter].qtext,token);
          token = strtok(NULL,",");
          char *arrowfields = strtok(token,">");
          for(int x = 0; x < 4; x++){
            multiq[MCA_Counter].options[x] = (char*)calloc(strlen(arrowfields),sizeof(char));
              strcpy(multiq[MCA_Counter].options[x],arrowfields);
            arrowfields = strtok(NULL,">");
          }
          multiq[MCA_Counter].answer = atoi(getfield(line,5)); 
          //printf("Parsed Line \nNumber: %i, Text:, %s, and Answer is number %i (%s) \n",multiq[MCA_Counter].qnum,multiq[MCA_Counter].qtext,multiq[MCA_Counter].answer,
          //       multiq[MCA_Counter].options[multiq[MCA_Counter].answer]);
          MCA_Counter = MCA_Counter + 1;
      }
        else if (strcmp(token,"Code")==0){
          token = strtok(NULL,",");
          codeq[Code_Counter].qnum = atoi(token);
          token = strtok(NULL,",");
          codeq[Code_Counter].lang = (char*)calloc(strlen(Language),sizeof(char));
            memcpy(codeq[Code_Counter].lang,Language,strlen(Language));
          codeq[Code_Counter].qtext = (char*)calloc(strlen(token),sizeof(char));
            strcpy(codeq[Code_Counter].qtext,token);
          token = strtok(NULL,",");
          for (int x = 0; x < 3; x ++){
            char *argfields = strtok(token,">");  
            codeq[Code_Counter].inputs[x] = (char*)calloc(strlen(argfields),sizeof(char));
              strcpy(codeq[Code_Counter].inputs[x],argfields);
              argfields = strtok(NULL,",");}
          for (int x = 0; x < 3; x++){
            char *argfields = strtok(token,">"); 
            codeq[Code_Counter].outputs[x] = (char*)calloc(strlen(argfields),sizeof(char));
              strcpy(codeq[Code_Counter].outputs[x],argfields);
          }
          Code_Counter++;
        }
       free(tmp);
    }
    printf("Parsisng successs!");
    struct parsedcsv returnstruct;
    memcpy(returnstruct.programqs,codeq,sizeof(codeq));
    memcpy(returnstruct.multi_choiceqs,multiq,sizeof(multiq));
    return returnstruct;
} 
  
  
// function to write a question and its answer to a file
void write_question_and_answer(char* file_path, char* question, char* answer){
    FILE* file = fopen(file_path, "a");
    fprintf(file, "Q: %s\nA: %s\n\n", question, answer);
    fclose(file);
}

// function to read a random question and its answer from a file
void read_random_question_and_answer(char* file_path){
    FILE* file = fopen(file_path, "r");
    char line[1000];
    char question[1000];
    char answer[1000];
    int question_found = 0;
    int multiple_choice = 0;
    int answer_found = 0;
    int count = 0;

    // loop through the file to find the questions and answers
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Q: ") != NULL) {
            question_found = 1;
            answer_found = 0;
            if (rand() % ++count == 0) {
                strcpy(question, line + 3);
                fgets(line, sizeof(line), file);
                strcpy(answer, line + 3);
            }
        } else if (strstr(line, "A: ") != NULL) {
            answer_found = 1;
            question_found = 0;
        }
    }
    printf("%s%s\n", question, answer);
    fclose(file); // close the file
}

void search_and_print(char* file_path, char* keyword){
    FILE* file = fopen(file_path, "r");
    char line[1000];
    while (fgets(line, sizeof(line), file)) {
    #include <stdio.h>
    #include <stdlib.h>
    }
}

