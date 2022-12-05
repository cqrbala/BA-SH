#include "header.h"

void sig_func_assignment(char **words)
{
    int i = 1;
    int count = 0;
    int flag1 = 0;
    int flag2 = 0;
    int j_num, sg_num;
    while (words[i] != NULL)
    {
        count++;
        if (flag1 == 1 && flag2 == 1)
        {
            break;
        }
        if (flag1 == 0)
        {
            j_num = atoi(words[i]);
            if (j_num == 0)
            {
                printf("bash: Invalid job number\n");
                return;
            }
            flag1 = 1;
        }
        else if (flag2 == 0)
        {
            sg_num = atoi(words[i]);
            if (sg_num == 0)
            {
                printf("bash: Invalid signal number\n");
                return;
            }
            flag2 = 1;
        }
        i++;
    }

    if (flag1 == 0 || flag2 == 0)
    {
        printf("bash: Invalid input\n");
        return;
    }

    if (count > 2)
    {
        printf("bash: Invaild input\n");
        return;
    }

    // printf("%d %d\n", j_num, sg_num);

    for (int i = 0; i < MAX_POOL_SIZE; i++)
    {
        if (process_ledger[i].pid != -1)
        {
            if (process_ledger[i].job_num == j_num)
            {
                // printf("reached here\n");
                int x = kill(process_ledger[i].pid, sg_num);
                if (x == -1)
                {
                    printf("bash: signal error occurred\n");
                }
                return;
            }
        }
    }

    printf("bash: invalid job number specified\n");
    return;
}