// USAGE: write_question_and_answer <filepath> <languageused>

#define LINELENGTH 2024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//structure definitons probably going to have to use in diff program
struct multi_choiceq {
  int qnum;
  char* qtext;
  char* options[4]; //2d array
  int answer; //1 = a, 2 = b, 3 = c, 4 = d e.t.c. 
};

struct programq {
  int qnum;
  char* qtext;
  char* lang; //C99, Java or Python
  char* inputs[3];    //What is expected to go in
  char* outputs[3]; //What is expected to come out 
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

char* getfieldarrow(char* line, int num)
{
    char* tok;
    for (tok = strtok(line, ">");
            tok && *tok;
            tok = strtok(NULL, ">\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}



//int fileparser(filepath,language) {
// using as main for now, but shouldn't be starting place for server, better to call it as a function
int main(int argc, char *argv[]) {
   FILE *fp;
   char filename[20]; //track name of file variable
   char Language[20]; //track Language

   // get filename from server request
   if (argc != 3 ) {
    printf("Error too many or too little arguments\n Current argument count: %i",argc); usage();
   }
   strcpy(filename, argv[1]);
   strcpy(Language, argv[2]);
   fp = fopen(filename, "r"); // open file for reading
   if (fp == NULL){
    printf("Internal Error: Unable to open file %s",filename); usage(); 
   }
  
  char line[LINELENGTH];
  
  struct multi_choiceq multiq[200];
  struct programq codeq[200000];
  int MCA_Counter = 0;
  int Code_Counter = 0;
  while (fgets(line, LINELENGTH, fp))
    {
        char* tmp = strdup(line);

        //skip commented lines (lines start with hashes)
        if (tmp[0] == '#'){
          continue;
        }

        printf("Field 2 would be %s\n", getfield(tmp, 2));
        // NOTE strtok clobbers tmp (idk what this means stolen from stackoverflow code)
        
        //This if command segfaults the program IDK why
        //Removed when fixed
        if (strcmp(getfield(tmp,2),"MCA")){
          multiq[MCA_Counter].qnum = atoi(getfield(tmp,1));
          strcpy(multiq[MCA_Counter].qtext,getfield(tmp,3));
          for(int x = 0; x < 4; x++){
            multiq[MCA_Counter].options[x] = getfieldarrow(getfield(tmp,4),x);
          }
          multiq[MCA_Counter].answer = atoi(getfield(tmp,5)); 
          printf("Parsed Line \n, Number: %i, Text:, %s, and Answer of:%i\n",multiq[MCA_Counter].qnum,multiq[MCA_Counter].qtext,multiq[MCA_Counter].answer);
          MCA_Counter++;
        }
        else if (strcmp(getfield(tmp,2),"Code")){
          codeq[Code_Counter].qnum = atoi(getfield(tmp,1));
          strcpy(codeq[Code_Counter].lang,Language);
          strcpy(codeq[Code_Counter].qtext,getfield(tmp,3));
          for (int x = 0; x < 3; x ++){
            codeq[Code_Counter].inputs[x] = getfieldarrow(getfield(tmp,4),x);
            codeq[Code_Counter].outputs[x] = getfieldarrow(getfield(tmp,5),x);
        }
      }
      free(tmp);
    }
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

