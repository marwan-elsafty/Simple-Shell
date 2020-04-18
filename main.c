    #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_CHAR 1000
#define MAX_ARGS 100


int getInput();
void splitInput();
int isBuiltInCommands();
int isBackgroundCommand();
void executeCommands();
void executeBackgroundCommand();
void sigHandler();

char input[MAX_CHAR], *tokenizedArgs[MAX_ARGS];





/* Function to get input from user */
int getInput()
{
    fgets(input,MAX_CHAR,stdin);

    //if there's no input return 1
    if(strlen(input) == 0)
        return 1;

    else
        return 0;

}

/* Function that splits the input to separate words
    -commands and arguments- to help in their execution */
void splitInput()
{

    const char s[2] = " ";
    char *token;
    token = strtok(input, s);

    int i=0;
    while( token != NULL )
    {
        tokenizedArgs[i] = token;
        token = strtok(NULL, s);
        //the upcoming line omits the \n found in the end of a command
        tokenizedArgs[i][strcspn(tokenizedArgs[i], "\n")] = 0;
        if (strlen(tokenizedArgs[i]) == 0)
            i--;
        i++;
    }

    //Last string must be NULL for execvp() to execute
    tokenizedArgs[i] = NULL;

}

/*function that checks whether the command is a built in command
    or one that needs to be implemented
    implemented commands are: exit, cd */
int isBuiltInCommands()
{
    char * implementedCommands[2];
    int flag=0;
    implementedCommands[0] = "exit";
    implementedCommands[1] = "cd";

    for(int i=0 ; i < 2 ; i++)
    {
        if(strcmp(tokenizedArgs[0],implementedCommands[i]) == 0)
        {
            flag = i+1;
            break;
        }
    }

    //return 0 if it is a built in command
    if(flag == 0)
        return 0;
    else if(flag==1)
        return 1;
    else if(flag==2)
    {
        chdir(tokenizedArgs[1]);
        return 2;
    }


    return 0;
}





 /* function to check if the command is a background command
    by searching for a '&' in the command */
int isBackgroundCommand()
{
    for(int i=0; i < strlen(input); i++)
    {
        if(input[i] == '&')
            return 1;
    }
    return 0;
}

/* Function to execute system commands */
void executeCommands()
{
    // pid is the process ID
    int pid = fork();

    // when pid=-1 that means there is a failure in creating a child
    if(pid == -1)
    {
        printf("Failed to create a child!");
        return;
    }

    // when pid=0 that means the child is created and being executed
    else if(pid == 0)
    {
        if (execvp(tokenizedArgs[0], tokenizedArgs) < 0)
        {
            printf("\nFailed to execute Command..");
        }
        exit(0);
    }

    // when pid is +ve that means the execution is back to the parent
    else
    {
        wait(NULL);
        signal(SIGCHLD,sigHandler);
        return;
    }
}


/* Function to background commands */
    void executeBackgroundCommand()
    {
        // pid is the process ID
        int pid = fork();

        // when pid=-1 that means there is a failure in creating a child
        if(pid == -1)
        {
            printf("Failed to create a child!");
            return;
        }

        // when pid=0 that means the child is created and being executed
        else if(pid == 0)
        {
            if (execvp(tokenizedArgs[0], tokenizedArgs) < 0)
            {
                printf("\nFailed to execute Command..");
            }
            exit(0);
        }

        // when pid is +ve that means the execution is back to the parent
        else
        {
            signal(SIGCHLD,sigHandler);
            return;
        }

    }

/* a signal handler that writes in a log file
    when a child is terminated */
void sigHandler()
{
    FILE *fp ;
    fp = fopen("logFile.txt", "a");
    fprintf(fp, "%s\n", "Child process was terminated");
}




int main()
{

    while(1)
    {
        printf("Shell >");
        int noInp = getInput();

        // if the user didn't enter a command re-enter the loop to
        // let him re-write a command
        if(noInp)
            continue;

        // check if the command is a background command
        if(isBackgroundCommand())
        {
            splitInput();
            if(isBuiltInCommands()==0)
                executeBackgroundCommand();
            else if(isBuiltInCommands() == 1)
                exit(0);
        }
        else
        {
            splitInput();
            if(isBuiltInCommands()==0)
                executeCommands();
            else if(isBuiltInCommands() == 1)
                exit(0);
        }

     }

    return 0;
}
