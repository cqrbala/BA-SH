#include "header.h"

// signal handler function that calls the child handler
handler *install_signal(int signum, handler *handler)
{
    struct sigaction new_action, old_action;

    memset(&new_action, 0, sizeof(struct sigaction));
    new_action.sa_sigaction = handler;
    sigemptyset(&new_action.sa_mask);

    new_action.sa_flags = SA_RESTART | SA_SIGINFO;

    if (sigaction(signum, &new_action, &old_action) < 0)
    {
        write(2, "\nbash: signal encountered an error\n", strlen("bash: signal encountered an error"));
        exit(1);
    }
    return (old_action.sa_sigaction);
}

void child_handler(int sig, siginfo_t *info, void *vp)
{
    // printf("child\n");
    int status;
    int pid;
    int flag = 0;

    // detect the status of the child processes that have exited
    while ((pid = waitpid(-1, &status, WUNTRACED | WNOHANG)) > 0)
    {
        // iterate to find out the child
        for (int i = 0; i < MAX_POOL_SIZE; i++)
        {
            if (process_ledger[i].pid == pid)
            {
                if (WIFSTOPPED(status))
                {
                    printf("\n%s with pid = %d did not exit normally\n", process_ledger[i].name, pid);
                    // printf("don't worry\n");
                    //  children_count--;
                    break;
                }
                // if the exit status is 0, abnormal exit
                if (WIFEXITED(status) == 0)
                {
                    printf("\n%s with pid = %d did not exit normally\n", process_ledger[i].name, pid);
                    // printf("oops\n");
                    process_ledger[i].pid = -1;
                    strcpy(process_ledger[i].name, "");
                    process_ledger[i].job_num = 0;
                    children_count--;
                    flag = 1;
                    break;
                }
                // normal exit
                else
                {
                    printf("\n%s with pid = %d exited normally\n", process_ledger[i].name, pid);
                    process_ledger[i].pid = -1;
                    strcpy(process_ledger[i].name, "");
                    process_ledger[i].job_num = 0;
                    children_count--;
                    flag = 1;
                    break;
                }
            }
        }
    }
    // if you detect an exited process, update the prompt
    // if (flag == 1)
    // {
    //     prompt();
    //     // printf("pinoy\n");
    //     write(1, promptstring, strlen(promptstring));
    // }
    return;
}
void ctrlc_handler(int sig, siginfo_t *info, void *vp)
{
    // printf("ending\n");
    return;
}

void ctrlz_handler(int sig, siginfo_t *info, void *vp)
{
    // printf("entered here\n");
    // printf("%d\n", fg_process.pid);
    if (fg_process.pid == -1)
    {
        return;
    }
    // printf("reached past it\n");
    setpgid(fg_process.pid, 0);

    int x = kill(fg_process.pid, SIGSTOP);
    if (x < 0)
    {
        printf("bash: Failed to move process to background\n");
        return;
    }

    for (int i = 0; i < MAX_POOL_SIZE; i++)
    {
        if (process_ledger[i].pid == -1)
        {
            // printf("%d\n", fg_process.pid);
            process_ledger[i].pid = fg_process.pid;
            strcpy(process_ledger[i].name, fg_process.name);
            children_count++;
            process_ledger[i].job_num = children_count;
            strcpy(fg_process.name, "");
            break;
        }
    }
}

void background(char **words)
{
    // printf("%s\n", words[0]);
    //  if ~ is the first charac, replace with homedir
    if (words[0][0] == '~')
    {
        char command[SIZE];
        strcpy(command, homedir);
        strcat(homedir, &words[0][1]);
        words[0] = command;
    }

    // fork a new child process
    pid_t id = fork();

    if (id == -1)
    {
        printf("\nbash: Failed to run background process\n");
        return;
    }

    // child process
    if (id == 0)
    {
        // if bg process can be added:
        if (children_count < MAX_POOL_SIZE)
        {
            // change the group id
            // setpgrp();
            setpgid(0, 0);
            if (execvp(words[0], words) < 0)
            {
                printf("\nbash : command not found\n");
                // prompt();
                // write(1, promptstring, strlen(promptstring));
                exit(1);
            }
            exit(1);
        }
        // pool full
        else
        {
            printf("\nbash: Process can't be added due to pool saturation\n");
            // prompt();
            // write(1, promptstring, strlen(promptstring));
            exit(1);
        }
    }
    // parent process
    else
    {
        // return if pool full
        if (children_count >= MAX_POOL_SIZE)
        {
            return;
        }
        // add the bg process to the pool
        else
        {
            for (lli i = 0; i < MAX_POOL_SIZE; i++)
            {
                if (process_ledger[i].pid == -1)
                {
                    process_ledger[i].pid = id;
                    strcpy(process_ledger[i].name, words[0]);
                    children_count++;
                    process_ledger[i].job_num = children_count;
                    printf("[%lld] %d\n", children_count, id);
                    break;
                }
            }
        }
        return;
    }
}