i#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
   FILE *fp;
   char filename[20];

   // get filename from server request
   strcpy(filename, argv[1]);

   // open appropriate file based on filename
   if(strcmp(filename, "C-Programming_Questions.txt") == 0) { // if server requests file1.txt
      fp = fopen("C-Programming_Questions.txt", "r"); // open file1.txt for reading
   } else if(strcmp(filename, "JAVA_Programming_Questions.txt") == 0) { // if server requests file2.txt
      fp = fopen("JAVA_Programming_Questions.txt", "r"); // open file2.txt for reading
   } else if(strcmp(filename, "file3.txt") == 0) { // if server requests file3.txt
      fp = fopen("Python_Programming_Questions.txt", "r"); // open file3.txt for reading
   } else { // if server requests an invalid filename
      printf("Invalid filename\n"); // print error message
      exit(1); // exit program with error code
   }

void write_question_and_answer(char* file_path, char* question, char* answer){
    FILE* file = fopen(file_path, "a");
    fprintf(file, "Q: %s\nA: %s\n\n", question, answer);
    fclose(file);
}

void read_random_question_and_answer(char* file_path){
    FILE* file = fopen(file_path, "r");
    char line[1000];
    char question[1000];
    char answer[1000];
    int question_found = 0;
    int multiple_choice = 0;
    int answer_found = 0;
    int count = 0;
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
    fclose(file);
}

 while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "MC: ") != NULL) {
            multiple_choice_found = 1;
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
    fclose(file);
}

void search_and_print(char* file_path, char* keyword){
    FILE* file = fopen(file_path, "r");
    char line[1000];
    while (fgets(line, sizeof(line), file)) {nclude <stdio.h>
#include <stdlib.h>

int main() {
   FILE *fp;

   // Open the file
   fp = fopen("file.txt", "w+");

   // Check if the file is opened successfully
   if (fp == NULL) {
      printf("Error opening the file.\n");
      exit(1);
   }
        if (strstr(line, keyword) != NULL) {
            printf("%s", line);
            while (fgets(line, sizeof(line), file)) {
                if (isspace(line[0])) {
                    break;
                } else {
                    printf("%s", line);
                }
            }
        }
    }
    fclose(file);
}
