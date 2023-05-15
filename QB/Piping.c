#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024  // Size of the pipe buffer
#define READ_END 0  // Read end of the pipe
#define WRITE_END 1  // Write end of the pipe

#define MAX_LINE_LENGTH 1024  // Maximum length of a line in the input file

// Multiple-choice question structure
struct multi_choiceq {
    int qnum;  // Question number
    char qtext[100];  // Question text
    char options[4][10];  // Options for the question
    int answer;  // Correct answer
};

// Programming question structure
struct programq {
    int qnum;  // Question number
    char* qtext;  // Question text
    char* lang;  // Programming language for the question
    char* inputs[3];  // Inputs for the question
    char* outputs[3];  // Expected outputs for the question
};

// Function to parse a multiple-choice question from a string
void parse_multi_choiceq(char* str, struct multi_choiceq* q) {
    sscanf(str, "MCA %d<br>%[^<br>]<br>%[^<br>]<br>%[^<br>]<br>%[^<br>]<br>%[^<br>]<br>%d",
        &(q->qnum), q->qtext, q->options[0], q->options[1], q->options[2], q->options[3], &(q->answer));
}

// Function to parse a programming question from a string
void parse_programq(char* str, struct programq* q) {
    sscanf(str, "Code %d<br>%m[^<br>]<br>%m[^<br>]<br>%m[^<comma>]<br>%m[^<comma>]<br>%m[^<comma>]<br>%m[^<comma>]<br>%m[^<comma>]<br>%m[^<comma>]",
        &(q->qnum), &(q->qtext), &(q->lang), &(q->inputs[0]), &(q->inputs[1]), &(q->inputs[2]), &(q->outputs[0]), &(q->outputs[1]), &(q->outputs[2]));
}

int main(void) {
    char write_msg[BUFFER_SIZE];  // Buffer for writing to the pipe
    char read_msg[BUFFER_SIZE];  // Buffer for reading from the pipe
    int fd[2];  // File descriptors for the pipe
    pid_t pid;  // Process ID

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
        // Close the unused read end of the pipe
        close(fd[READ_END]);

        // Open the file containing the questions
        FILE* file = fopen("questions.txt", "r");
        if (file == NULL) {
            fprintf(stderr, "Failed to open file\n");
            return 1;
        }

        // Read questions from the file
        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strncmp(line, "MCA", 3) == 0) {
                // Parse a multiple-choice question and write it to the pipe
                struct multi_choiceq q1;
                parse_multi_choiceq(line, &q1);
                sprintf(write_msg, "%d<br>%s<br>%s<br>%s<br>%s<br>%s<br>%d", q1.qnum, q1.qtext, q1.options[0], q1.options[1], q1.options[2], q1.options[3], q1.answer);
                write(fd[WRITE_END], write_msg, strlen(write_msg) + 1);
            }
            else if (strncmp(line, "Code", 4) == 0) {
                // Parse a programming question and write it to the pipe
                struct programq q2;
                parse_programq(line, &q2);
                sprintf(write_msg, "%d<br>%s<br>%s<br>%s<br>%s<br>%s<br>%s<br>%s<br>%s", q2.qnum, q2.qtext, q2.lang, q2.inputs[0], q2.inputs[1], q2.inputs[2], q2.outputs[0], q2.outputs[1], q2.outputs[2]);
                write(fd[WRITE_END], write_msg, strlen(write_msg) + 1);
            }
        }

        // Close the write end of the pipe
        close(fd[WRITE_END]);
        fclose(file);
    }
    else { // child process
        // Close the unused write end of the pipe
        close(fd[WRITE_END]);

        // Read from the pipe
        while(read(fd[READ_END], read_msg, BUFFER_SIZE) > 0) {
            printf("read %s\n",read_msg);
        }

        // Close the read end of the pipe
        close(fd[READ_END]);
    }

    return 0;
}
