#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define READ_END 0
#define WRITE_END 1

struct multi_choiceq {
    int qnum;
    char qtext[100];
    char options[4][10]; //2d array
    int answer; //1 = a, 2 = b, 3 = c, 4 = d e.t.c.
};

struct programq {
    int qnum;
    char* qtext;
    char* lang; //C99, Java or Python
    char* inputs[3]; //What is expected to go in
    char* outputs[3]; //What is expected to come out
};

// Function to parse a multiple-choice question from a string
void parse_multi_choiceq(char* str, struct multi_choiceq* q) {
    sscanf(str, "%d<br>%[^<br>]<br>%[^<br>]<br>%[^<br>]<br>%[^<br>]<br>%[^<br>]<br>%d",
        &(q->qnum), q->qtext, q->options[0], q->options[1], q->options[2], q->options[3], &(q->answer));
}

// Function to parse a programming question from a string
void parse_programq(char* str, struct programq* q) {
    sscanf(str, "%d<br>%m[^<br>]<br>%m[^<br>]<br>%m[^<comma>]<br>%m[^<comma>]<br>%m[^<comma>]<br>%m[^<comma>]<br>%m[^<comma>]<br>%m[^<comma>]",
        &(q->qnum), &(q->qtext), &(q->lang), &(q->inputs[0]), &(q->inputs[1]), &(q->inputs[2]), &(q->outputs[0]), &(q->outputs[1]), &(q->outputs[2]));
}

int main(void) {
    char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];
    int fd[2];
    pid_t pid;

    // Create the pipe
    if (pipe(fd) == -1) {
        fprintf(stderr,"Pipe failed");
        return 1;
    }

    // Fork a child process
    pid = fork();

    if (pid < 0) { // error occurred
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    if (pid > 0) { // parent process
        // Close the unused end of the pipe
        close(fd[READ_END]);

        // Create a multiple-choice question and write it to the pipe
        struct multi_choiceq q1 = {1, "What is 2 + 2?", {"1", "2", "3", "4"}, 4};
        sprintf(write_msg, "%d<br>%s<br>%s<br>%s<br>%s<br>%s<br>%d", q1.qnum, q1.qtext, q1.options[0], q1.options[1], q1.options[2], q1.options[3], q1.answer);
        write(fd[WRITE_END], write_msg, strlen(write_msg) + 1);

        // Create a programming question and write it to the pipe
        struct programq q2 = {1, "What is a loop?", "C99", {"input1<comma>input2<comma>
