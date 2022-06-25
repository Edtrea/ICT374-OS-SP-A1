#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    const char* filename = "temp.txt";
    int i,count = argc;
    pid_t pid[argc];
    pid_t waitStatus = 0, wpid;
    FILE* file;
    char* line=NULL;
    size_t len=0;

        //Delete the temp file for storing the pid and command
        //temp file is used so that pid could be used to identify which command is ran for a particular process
    remove(filename);
        //Check if argument was provided
    if(argc < 2)
    {
        printf("No argument is provided\n");
    }

        //Read command line from command line arguments Lab2 e8
    for (i = 1;i < argc;++i)
    {
            //For each line create a child process and run the command line
        pid[i-1] = fork();           
        if (pid[i-1] == 0) // child
        {
                //create/open the temp file that will store child pid and command
            FILE* file = fopen(filename, "a+");
            fprintf(file, "%d\n%s\n", getpid(), argv[i]);
            fclose(file);

            if(execl(argv[i],argv[i], (char *) 0) == -1)
            {
                printf("Command %s has error executing\n", argv[i]);
                exit(1);
            }
        }

        if(pid[i-1] < 0)   //error
        {
            printf("Command %s has encountered error while forking\n", argv[i]);
        }

    }

    while(count > 1)
    {
        wpid = wait(&waitStatus);
            //Check if Process exited normally
            //Once command is completed print 'completed successfully' before exiting
            //If command is not successful print 'not completed succuessfully'
            //Loops through tempfile to compare wpid
            //Once pid matches the next line is the command
        file = fopen(filename, "r");
        while(getline(&line, &len, file) != -1)
        {
            if (WIFEXITED(waitStatus))
            {
                if(WEXITSTATUS(waitStatus) == 0)
                {
                        //
                    if(atoi(line) == wpid)
                    {
                        getline(&line, &len, file);
                        line[strlen(line)-1] = '\0';
                        printf("Command %s has been completed successfully\n", line);
                    }
                }
                else
                {
                    if(atoi(line) == wpid)
                    {
                        getline(&line, &len, file);
                        line[strlen(line)-1] = '\0';
                        printf("Command %s has not been completed successfully\n", line);
                    }
                }
            }
            else
            {  
                if(atoi(line) == wpid)
                {
                    getline(&line, &len, file);
                    line[strlen(line)-1] = '\0';
                    printf("Command %s has not been completed successfully\n", line);
                }
            }
            
        }
        fclose(file);
        count--;
    }

        //print 'All done, bye-bye' once all child terminated before terminating parent
    printf("All done, bye-bye\n");
    exit(0);
}