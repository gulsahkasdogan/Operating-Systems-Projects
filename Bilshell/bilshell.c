#include <readline/readline.h>
#include <readline/history.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>

#define NUM_OF_LETTERS 1000 //total number of letters including space
#define NUM_OF_COMMANDS 100//number of space seperated words in command
void commandProcessing(char* str, int N);

//Generates M random variables and fills string.
void generateRandom(char *string, int M){
    static const char alphanumeric[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < M; ++i) {
        string[i] = alphanumeric[rand() % (sizeof(alphanumeric) - 1)];
    }
}

//Built-in producer command, generates M random alphanumerics and displays them N by N
void producer(int M, int N){
    char string[M];
    generateRandom(string, M);
    int remaining = M;
    int count = 0;
    while(remaining > 0){
        write(1, string, (size_t) N); //ins
        remaining -= N;
        count++;
    }
}

//Built-in consumer command, reads M alphanumerics from standart input
void consumer(int M, int N){
    int remaining = M;
    char buffer[N];
    int count = 0;
    while(remaining > 0){
        read(0, buffer, sizeof(buffer));
        remaining -= N;
        count++;
    }
}

//Displays a welcome message
void initialize(){
    //print a message according to time of the day
    time_t rawtime;
    time(&rawtime);
    struct tm* time_info = localtime(&rawtime);
    int time = time_info->tm_hour;
    if(time < 6)
        printf("Good night!");
    else if(time < 12) //>6
        printf("Good Morning Sunshine!");
    else if(time < 17) //>12
        printf("Good Afternoon!");
    else
        printf("Good Evening!");
    char *username = getenv("USER"); //username from system
    printf("\nWelcome to bilshell, @%s\n", username);
}

//Reads input of user by using buffer
int readInput(char* str){
    char* buffer;
    buffer = readline("bilshell-$ "); //leak
/*	printf(">>> ");
	fgets(buffer, NUM_OF_LETTERS, stdin);
	buffer[sizeof(buffer)-1] = '\0';
*/
    if(strlen(buffer) == 0){
        return 1;
    }
    else{
        add_history(buffer); 	//add this command to history so that
        //you can go back with arrow keys
        strcpy(str, buffer);
        //printf("command: %s", str);
        free(buffer);
        return 0;
    }
}

//Prints current directory
void printCurrentDir(){
    char dir[1024];
    getcwd(dir, sizeof(dir));
    printf("\ncurrent directory: %s\n ", dir);
}

//we need to be able to read through space seperated commands
//space parser
void parseSpace(char *str, char **parsed){
    int i = 0;
    char* param;
    while(i < NUM_OF_COMMANDS){
        //you can accept at most NUM_OF_COMMANDS commands.
        param = strtok(str, "\n\r\a");
        parsed[i] = strsep(&str, " ");
        //note: this is the exact order of if blocks! --segmentation fault
        if(parsed[i] == NULL){
            //means you are done with parsing
            break;
        }
        if(strlen(parsed[i]) > 0){
            i++; //means there might be commands to process -still
        }
    }
}

//return 0 if not batch
int isBatch(char** parsed){
    for(int i = 0; i < 3; i++){
        if(parsed[i] == NULL)
            return 0;
    }

    if(strcmp(parsed[0], "bilshell") == 0){
        //check if second one is a number
        int N = atoi(parsed[1]);
        if(N == 0)
            return 0;
        else{
            //check if it is a text file
            //second input is valid you can check the third
            char* filename = parsed[2];
            char* descriptor = ".txt";
            char* found = strstr(filename, descriptor);
            if(found == NULL){
                printf("\nWARNING: You should provide a txt file\n");
                return 0;
            }
            if(strcmp(found, descriptor) == 0){
                //text  file is correct
                //printf("%s\n%s\n%s", parsed[0], parsed[1], parsed[2]);
                return N;
            }
        }
    }
    else
        return 0;
}

//returns 0 if it is not a built in command, 1 if it is a built-in command
//built in executer
int executeBuiltIn(char** command, int N){
    //command comes space parsed.
    //some commands are not executed by using cvp, these will be added to here
    int numberOfCommands = 5;
    int i;
    char* builtIn[numberOfCommands];
    builtIn[0] = "exit";
    builtIn[1] = "cd";
    builtIn[2] = "help";
    builtIn[3] = "producer";
    builtIn[4] = "consumer";
    char** rest_of_command = NULL;

    for(i = 0; i< numberOfCommands; i++){
        if(strcmp(*command, builtIn[i]) == 0){
            //they are same
            break;
        }
    }
    switch(i){
        case 0: printf(" Bye!\n"); exit(0); break;
        case 1: chdir(command[1]); break;
        case 2: printf("Welcome bilshell!\nType exit to terminate shell\n**Please checkout the README file for details\n"); break;
        case 3: //producer
            if(command[1] != NULL ){
                int size = atoi(command[1]);
                if(size > 0)
                    producer(size, N);
            }
            break;
        case 4: //consumer
            if(command[1] != NULL){
                int size = atoi(command[1]);
                if(size > 0)
                    consumer(size, N);
            }
            break;
        default: break;
    }
    if(i < numberOfCommands)
        return 1; //it is one of our functions
    else
        return 0; //it is not a built in
}

//built in commands and simple commands are executed here
//singular executer
int executeSingular(char** parsed, int N){
    //execute arguments for singular command
    if(executeBuiltIn(parsed, N))
        return 2;
    pid_t pid = fork();
    if(pid < 0){
        //failed
        printf("ERROR: Creation of child process!\n");
        return 0;
    }
    else if(pid == 0){
        //child
        int executed = execvp(*parsed, parsed);
        if(executed < 0){
            printf("WARNING: Could not execute command\n");
        }
        exit(0);
    }else{
        //parent
        wait(NULL); //wait for child process
        return 1;
    }
}

//pipe executer
void executePipedArg(char** piped1, char** piped2, int N){
    //piped1 --> child1 process
    //piped2 --> child2 process
    int pipe1_fd[2];
    int pipe2_fd[2]; //file descriptors
    ssize_t nbytes = 0;
    ssize_t read_byte = 0;
    ssize_t write_byte = 0;

    pid_t child1, child2;
    char buffer[N];

    if(pipe(pipe1_fd) < 0 || pipe(pipe2_fd) < 0){
        //either of them had a problem during initialization
        printf("ERROR: Pipe initialization!\n");
        return;
    }
    child1 = fork();
    if(child1 == 0){
        //child1
        //printf("Child1 executes \n");
        dup2(pipe1_fd[1], 1);
        //execvp(piped1[0], piped1);
        executeSingular(piped1, N);

        exit(0);
    }
    else if(child1 > 0){
        //parent
        close(pipe1_fd[1]); //write end of pipe1
        wait(NULL);

        while(nbytes = read(pipe1_fd[0], buffer, (size_t)N) > 0){
            write_byte += write(pipe2_fd[1], buffer, (size_t) N);
            read_byte += nbytes;
        }

        close(pipe1_fd[0]);
        close(pipe2_fd[1]);

        child2 = fork();
        if(child2 == 0){
            //child2
            //printf("Child2 executes\n");
            dup2(pipe2_fd[0], 0);
            //execvp(piped2[0], piped2);
            executeSingular(piped2, N);

            exit(0);
        }
        else if(child2 > 0){
            //parent
            close(pipe2_fd[0]);
            //printf("Child2 terminated\n");
            wait(NULL);
            printf("read-call-count: %ld\n", read_byte);
            printf("character-count: %ld\n", write_byte); //total number rof bytes written
		
		//printf("%ld\n", read_byte);
            //printf("character-count: %ld\n", write_byte); //total number rof bytes written
        }
    }
}

//will return 0 if command is not composite (ie does not need pipe)
int parsePiped(char* str, char** piped1, char** piped2){
    piped2[0] = strsep(&str, "|"); //first part of composite command
    if(piped2[0] != NULL){
        piped2[1] = strsep(&str, "|"); //second part of composite command
        if(piped2[1] != NULL){
            //command is composite
            //process for pipe
            parseSpace(piped2[0], piped1);
            parseSpace(piped2[1], piped2);
            return 1;
        } else
            return 0;
    }
    else
        return 0; //not even singular command
}

//Batch mode executer, reads commands from a file rather than stdin
void executeBatch(char** parsed, int N){
    //open the file with the given name
    char* filename = parsed[2];
    FILE* fp = NULL;
    char buffer[50]; //maximum number of letters you can read
    fp = fopen(filename, "r");
    char* spaceParsed[NUM_OF_COMMANDS];
    char* textCmds[NUM_OF_COMMANDS];
    int newN = 1;
    char* param;

    if(fp == NULL){
        printf("\nWARNING: File does not exist!\n");
        return;
    }
    else{
        char c;
        int count = 0;
        for (c = getc(fp); c != EOF; c = getc(fp)){
            if (c == '\n')
                count = count + 1;
        }
        //printf("Number of lines: %d ", count);
        fclose(fp);
        fp = fopen(filename, "r");

        for(int i = 0; i < count; i++){
            fgets(buffer, sizeof(buffer), fp);
            //printf(buffer);
            buffer[sizeof(buffer)-1] = '\0';

            param = strtok(buffer, "\n\r\a");
            newN = atoi(parsed[1]);
            if(newN <= 0) //not valid
                newN = N;
            if(param != NULL)
                commandProcessing(param, newN);
        }
        fclose(fp);
    }
}

//Checks the type of command(singular, compound or batch), calls related executer.
void commandProcessing(char* str, int N){
    char* spaceParsed[NUM_OF_COMMANDS];
    char* parsed1[NUM_OF_COMMANDS];
    char* parsed2[NUM_OF_COMMANDS];

    char pipeInput[NUM_OF_LETTERS]; //input to pipe
    char timeInput[NUM_OF_LETTERS]; //input to time
    strcpy(pipeInput, str);
    strcpy(timeInput, str);

    parseSpace(str, spaceParsed); //now all elements are in spaceParsed array
    int batch = isBatch(spaceParsed);
    int isPipe = parsePiped(pipeInput, parsed1, parsed2);

    if(spaceParsed[0] == NULL)
        return;

    if(batch > 0){
        //printf("I am a batch\n");
        executeBatch(spaceParsed, N);
    } else if(isPipe > 0){
        //printf("I am a pipe\n");
        executePipedArg(parsed1, parsed2, N);
    }
    else{
        //printf("I am a simple command\n");
        executeSingular(spaceParsed, N);
    }
/*
    for(int i = 0; i< NUM_OF_COMMANDS; i++){
        if(spaceParsed[i] != NULL)
            printf("%s\n", spaceParsed[i]);
        else
            break;
    }
    */

}

int main(int argc, char** argv) {

    char *escape = "eject"; //special escape command
    char inputString[NUM_OF_LETTERS];
    int N = 1;

    printf("Starting shell!\n");
    initialize();

    printf("Enter N: ");
    scanf("%d", &N);
    printf("N is %d", N);


    while(1){
        printCurrentDir();
        int isEmpty = readInput(inputString);
        if(strcmp(inputString, escape) == 0){
            //unexpected exit
            break;
        }
        else if(isEmpty){
            //means you entered space, tab etc...
            continue;
        }
        else{
            //process
            commandProcessing(inputString, N);
        }
    }
    printf("See you!\n\n");
    return 0;
}
