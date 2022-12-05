#include "header.h"

void fg_function(char **words)
{
    int i = 1;
    int count = 0;
    int j_no;
    int flag1 = 0;
    while (words[i] != NULL)
    {
        count++;
        if (flag1 == 0)
        {
            j_no = atoi(words[i]);
            if (j_no == 0)
            {
                printf("bash: Invalid input\n");
                return;
            }
            flag1 = 1;
        }
        i++;
    }

    if (count > 1)
    {
        printf("bash: too many arguments\n");
        return;
    }

    pid_t curr_process = getpid();
    for (int x = 0; x < MAX_POOL_SIZE; x++)
    {
        if (process_ledger[x].pid != -1)
        {
            if (process_ledger[x].job_num == j_no)
            {
                signal(SIGTTOU, SIG_IGN);
                signal(SIGTTIN, SIG_IGN);
                tcsetpgrp(STDIN_FILENO, getpgid(process_ledger[x].pid));

                fg_process.pid = process_ledger[x].pid;
                strcpy(fg_process.name, process_ledger[x].name);
                fg_process.job_num = 1;

                // process_ledger[x].pid = -1;
                // process_ledger[x].job_num = 0;
                // strcpy(process_ledger[x].name, "");
                // children_count--;

                int z = kill(process_ledger[x].pid, SIGCONT);
                if (z == -1)
                {
                    printf("bash: error in running the process in foreground\n");
                    return;
                }

                int status;

                waitpid(process_ledger[x].pid, &status, WUNTRACED);
                fg_process.pid = -1;
                tcsetpgrp(STDIN_FILENO, getpgid(curr_process));
                signal(SIGTTOU, SIG_DFL);
                signal(SIGTTIN, SIG_DFL);

                if (WIFEXITED(status))
                {
                    process_ledger[x].pid = -1;
                    process_ledger[x].job_num = 0;
                    strcpy(process_ledger[x].name, "");
                    children_count--;
                }
                return;
            }
        }
    }
    printf("bash: Invalid input here\n");
    return;
}