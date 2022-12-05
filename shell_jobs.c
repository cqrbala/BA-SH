#include "header.h"

void jobs(char **words)
{
    // for (int i = 0; i < MAX_POOL_SIZE; i++)
    // {
    //     if (process_ledger[i].pid != -1)
    //     {
    //         printf("%d %s %d\n", process_ledger[i].job_num, process_ledger[i].name, process_ledger[i].pid);
    //     }
    // }
    int r = 0;
    int s = 0;
    int i = 1;
    while (words[i] != NULL)
    {
        if (words[i][0] == '-')
        {
            if (strchr(words[i], 'r') != NULL)
            {
                r++;
            }
            else if (strchr(words[i], 's') != NULL)
            {
                s++;
            }
        }
        i++;
    }

    if (r > 1 || s > 1)
    {
        printf("bash: Invalid input\n");
        return;
    }
    if (r == 0 && s == 0)
    {
        r = 1;
        s = 1;
    }

    struct process_info *sortedbgs[MAX_POOL_SIZE] = {NULL};
    int count = 0;
    for (int i = 0; i < MAX_POOL_SIZE; i++)
    {
        if (process_ledger[i].pid != -1)
        {
            sortedbgs[count] = (struct process_info *)malloc(1 * sizeof(struct process_info));
            sortedbgs[count]->pid = process_ledger[i].pid;
            sortedbgs[count]->job_num = process_ledger[i].job_num;
            strcpy(sortedbgs[count]->name, process_ledger[i].name);
            count++;
        }
    }

    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < count - 1; j++)
        {
            if (strcmp(sortedbgs[j]->name, sortedbgs[j + 1]->name) > 0)
            {
                int tempnum = sortedbgs[j]->job_num;
                pid_t temppid = sortedbgs[j]->pid;
                char tempname[1000];
                strcpy(tempname, sortedbgs[j]->name);

                sortedbgs[j]->job_num = sortedbgs[j + 1]->job_num;
                sortedbgs[j]->pid = sortedbgs[j + 1]->pid;
                strcpy(sortedbgs[j]->name, sortedbgs[j + 1]->name);

                sortedbgs[j + 1]->job_num = tempnum;
                sortedbgs[j + 1]->pid = temppid;
                strcpy(sortedbgs[j + 1]->name, tempname);
            }
        }
    }

    char procpath[100];
    char procdata[3000];
    FILE *fp_proc;
    for (int i = 0; i < count; i++)
    {
        int pid = sortedbgs[i]->pid;
        sprintf(procpath, "/proc/%d/stat", pid);

        fp_proc = fopen(procpath, "r");
        if (!fp_proc)
        {
            printf("bash: Error occured\n");
            return;
        }

        fread(procdata, 3000, 1, fp_proc);
        fclose(fp_proc);

        char *tokenizing = strtok(procdata, " ");
        tokenizing = strtok(NULL, " ");
        tokenizing = strtok(NULL, " ");

        // printf("%s\n", tokenizing);

        if (r == 1 && s == 1)
        {
            if (tokenizing[0] == 'T')
            {
                printf("[%d] Stopped %s [%d]\n", sortedbgs[i]->job_num, sortedbgs[i]->name, (int)sortedbgs[i]->pid);
            }
            else
            {
                printf("[%d] Running %s [%d]\n", sortedbgs[i]->job_num, sortedbgs[i]->name, (int)sortedbgs[i]->pid);
            }
        }
        else if (r == 1 && s == 0)
        {
            if (tokenizing[0] != 'T')
            {
                printf("[%d] Running %s [%d]\n", sortedbgs[i]->job_num, sortedbgs[i]->name, (int)sortedbgs[i]->pid);
            }
        }
        else if (r == 0 && s == 1)
        {
            if (tokenizing[0] == 'T')
            {
                printf("[%d] Stopped %s [%d]\n", sortedbgs[i]->job_num, sortedbgs[i]->name, (int)sortedbgs[i]->pid);
            }
        }
    }
}