#include "header.h"

void pinfo(char **words)
{
    int i = 1;
    int count = 0;
    pid_t arg_pid;
    int flag1 = 0;
    int curr_groupid;
    int grp_id_terminal = tcgetpgrp(STDOUT_FILENO);
    while (words[i] != NULL)
    {
        count++;
        if (flag1 == 0)
        {
            arg_pid = atoi(words[i]);
            if (arg_pid == 0)
            {
                printf("bash: Invalid pid\n");
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
    if (flag1 == 0)
    {
        arg_pid = getpid();
    }

    curr_groupid = getpgid(arg_pid);

    char procpath[100];
    char procdata[5000];
    char *details[50] = {NULL};
    FILE *fp_proc;

    sprintf(procpath, "/proc/%d/stat", arg_pid);

    fp_proc = fopen(procpath, "r");
    if (!fp_proc)
    {
        printf("bash: Error opening proc file\n");
        return;
    }
    fread(procdata, 5000, 1, fp_proc);
    char *word = strtok(procdata, " ");

    for (int i = 0; i < 50; i++)
    {
        details[i] = word;
        word = strtok(NULL, " ");
    }

    printf("pid : %d\n", arg_pid);
    printf("process status : %s%c\n", details[2], curr_groupid == grp_id_terminal ? '+' : ' ');
    printf("process memory: %s\n", details[22]);

    char exepath[500];
    int path_itr = 0;
    for (int x = 0; x < strlen(details[1]); x++)
    {
        if (details[1][x] == '(' || details[1][x] == ')')
        {
            continue;
        }
        exepath[path_itr] = details[1][x];
        path_itr++;
    }
    exepath[path_itr] = '\0';
    printf("executable path: %s\n", exepath);
}