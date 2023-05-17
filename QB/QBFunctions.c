#include "qb.h"
// USAGE: write_question_and_answer <filepath> <languageused>

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
    printf("\nParsisng successs!\n");
    struct parsedcsv returnstruct;
    if (codeq[Code_Counter-1].qnum > multiq[MCA_Counter-1].qnum){
      returnstruct.numq = codeq[Code_Counter-1].qnum;}
    else{ returnstruct.numq=multiq[MCA_Counter].qnum;}
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

// function to read a random q and 
int* randomQ(int amount,int upper, int *randq){
  srand(time(0));
  for (int x = 0;x != amount;x++){
    randq[x] = (rand() % (upper));
  }
  return randq;
  
}


//char* lowerCaser(char *string){
//  for(int i = 0; string[i]; i++){
//    string[i] = tolower(string[i]);
//  }
//  return string;
//}
