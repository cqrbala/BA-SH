#include "header.h"

void bg_function(char **words)
{
    int i = 1;
    int count = 0;
    int flag1 = 0;
    int j_no;

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

    for (int x = 0; x < MAX_POOL_SIZE; x++)
    {
        if (process_ledger[x].pid != -1)
        {
            if (process_ledger[x].job_num == j_no)
            {
                int z = kill(process_ledger[x].pid, SIGCONT);
                if (z == -1)
                {
                    printf("bash: error occurred\n");
                }
                return;
            }
        }
    }

    printf("bash: Invalid input\n");
    return;
}