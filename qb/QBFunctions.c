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
          multiq[MCA_Counter].qnum = MCA_Counter;

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
          codeq[Code_Counter].qnum = Code_Counter;

          token = strtok(NULL,",");

          codeq[Code_Counter].lang = (char*)calloc(strlen(Language),sizeof(char));
            memcpy(codeq[Code_Counter].lang,Language,strlen(Language));
          codeq[Code_Counter].qtext = (char*)calloc(strlen(token),sizeof(char));
            strcpy(codeq[Code_Counter].qtext,token);
          token = strtok(NULL,",");
          char str[strlen(token)];
          strcpy(str,token);
          char argfields[BUFFER_SIZE];
          for (int x = 0; x < 3; x ++){
            strcpy(argfields,"");
            delimfinder(str,argfields,'>',x);
            codeq[Code_Counter].inputs[x] = (char*)calloc(strlen(argfields),sizeof(char));
              strcpy(codeq[Code_Counter].inputs[x],argfields);}
          for (int x = 0; x < 3; x++){
            strcpy(argfields,""); 
            delimfinder(str,argfields,'>',x);
            char *argfields = strtok(str,">"); 
            codeq[Code_Counter].outputs[x] = (char*)calloc(strlen(argfields),sizeof(char));
              strcpy(codeq[Code_Counter].outputs[x],argfields);
          }
          Code_Counter++;
        }
       free(tmp);
    }
    printf("\nParsisng successs! MCA: %i, Code: %i\n",MCA_Counter,Code_Counter);
    struct parsedcsv returnstruct;
    memcpy(returnstruct.programqs,codeq,sizeof(codeq));
    memcpy(returnstruct.multi_choiceqs,multiq,sizeof(multiq));
    returnstruct.cnum = Code_Counter;
    returnstruct.mcanum = MCA_Counter;
    returnstruct.numq = Code_Counter + MCA_Counter + 2;
    return returnstruct;
} 
  
  
// function to write a question and its answer to a file
void write_question_and_answer(char* file_path, char* question, char* answer){
    FILE* file = fopen(file_path, "a");
    fprintf(file, "Q: %s\nA: %s\n\n", question, answer);
    fclose(file);
}

// function to read a random q and 
void randomQ(int amount,int upper, int *randq){
  int *ranq = malloc(amount);
  srand(time(0));
  for (int x = 0;x != amount;x++){
    randq[x] = (rand() % (upper));
    //TODO turn this loop int a function so we can summon and make sure 100% values in list don't repeat
    for (int y=0; y != x;y++){
      if (randq[x] == randq[y])
         randq[x] = (rand() % (upper));
    }
  }
  return;  
}

void randomLoop(int *randq,int randvalue,int length,int upper,int loopnumber){
  loopnumber=loopnumber+1;
  if (loopnumber > 20){ return; }
  randvalue = (rand()%(upper));
  for (int y=0; y != length;y++){
      randq[y] = (rand()%(upper));
      if (randq[y] == randvalue){
        randomLoop(randq,randvalue,length,upper,loopnumber);
    }
  }
}

char *exec(char *command,char *Output){
  FILE *fp;
  fp = popen(command,"r");
  if (fp == NULL){
    printf("Failed to run command\n");
    exit(1);
  }
  
  
  char path[BUFFER_SIZE];
  while (fgets(path,sizeof(path),fp)!=NULL){
    strcat(Output,path);
    printf("%s",path);
  }

  pclose(fp);
  return Output;
}

char *delimfinder(char *targetstring,char *buffer,char delim,int xnumber){
  int number = xnumber + 1;
  int counter = 0;
  int end = 0;
  int start = 0;
  for (;counter<(number+1);end++){
    if (targetstring[end]=='\0')
        break;
    if (targetstring[end]==delim){
      counter++;
      if (counter == number+1){
        break;}
      start = end+1;
    } 
  }

  strcpy(buffer,targetstring);
  buffer[end] = '\0';
  memmove(buffer,buffer+start,strlen(buffer));
  return buffer;
}

//char* lowerCaser(char *string){
//  for(int i = 0; string[i]; i++){
//    string[i] = tolower(string[i]);
//  }
//  return string;
//}
