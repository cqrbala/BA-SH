#include "header.h"

void foreground(char **words)
{
    // function to get the start time of the fg
    struct timeval starttime;
    gettimeofday(&starttime, NULL);

    // replace ~ with home dir and get the path after it
    if (words[0][0] == '~')
    {
        char command[SIZE];
        strcpy(command, homedir);
        strcat(homedir, &words[0][1]);
        words[0] = command;
    }

    // fork a new process
    pid_t id = fork();

    if (id == -1)
    {
        printf("Failed to run foreground process\n");
        return;
    }

    if (id == 0)
    {
        // execvp if child
        if (execvp(words[0], words) < 0)
        {
            printf("command not found\n");
            exit(1);
        }
        exit(1);
    }
    else
    {
        // wait until termination of child if parent process
        int status;
        fg_process.job_num = 1;
        strcpy(fg_process.name, words[0]);
        fg_process.pid = id;
        int terminatedstat = waitpid(id, &status, WUNTRACED);
        fg_process.job_num = 0;
        strcpy(fg_process.name, "");
        fg_process.pid = -1;
    }

    // get the stop time of the fg
    struct timeval endtime;
    gettimeofday(&endtime, NULL);

    // if the time elapsed is >= 1, set the time flag to print in the prompt
    timeelapsed = endtime.tv_sec - starttime.tv_sec;
    if (timeelapsed >= 1)
    {
        timeflag = 1;
    }
    return;
}