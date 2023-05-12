// USAGE: write_question_and_answer <filepath> <languageused>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char Usage_msg[] = ("USAGE: ./Write_Questions <file path> <Langauge used>\n");

void usage(){
  printf("\n%s\n",Usage_msg);
  exit(EXIT_FAILURE);
}

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
    printf("Internal Error: Unable to open file %s, file may be missing",filename); usage(); 
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

