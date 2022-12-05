#include "header.h"

void history_write(char **words)
{
    // create the absolute path for histor.txt
    strcpy(historypath, homedir);
    strcat(historypath, "/");
    strcat(historypath, "history.txt");

    // create the absolute path for temphistory.txt
    strcpy(temphistorypath, homedir);
    strcat(temphistorypath, "/");
    strcat(temphistorypath, "temphistory.txt");

    lli history_count = 0;
    lli i = 0;
    tempcommand[0] = '\0';
    // get the entire input string
    while (words[i] != NULL)
    {
        strcat(tempcommand, words[i]);
        strcat(tempcommand, " ");
        i++;
    }
    strcat(tempcommand, "\n");

    FILE *fptr = fopen(historypath, "r");
    if (fptr == NULL)
    {
        printf("history: Error opening file \n");
        return;
    }
    // count the number of commands in historr file atm
    while (fgets(historycommand, sizeof(historycommand), fptr) != NULL)
    {
        strcpy(lasthistorycommand, historycommand);
        history_count++;
    }
    fclose(fptr);

    // if it is < than 20, write the commands on to the file if it is different from the last command
    if (history_count < 20)
    {
        if (strcmp(tempcommand, lasthistorycommand))
        {
            FILE *fptr = fopen(historypath, "a");
            if (fptr == NULL)
            {
                printf("history: Error opening file \n");
                return;
            }
            fputs(tempcommand, fptr);
            fclose(fptr);
        }
    }
    // if there are already 20 commands
    else
    {
        if (strcmp(tempcommand, lasthistorycommand))
        {
            FILE *fptr = fopen(historypath, "r");
            FILE *fptr2 = fopen(temphistorypath, "a");
            if (fptr == NULL)
            {
                printf("history: Error opening file\n");
                return;
            }
            if (fptr2 == NULL)
            {
                printf("history: Error opening file\n");
                return;
            }

            // copy the last 19 commands to the temp file
            for (int z = 0; z < 20; z++)
            {
                char *jezos = fgets(historycommand, sizeof(historycommand), fptr);
                if (z == 0)
                {
                    continue;
                }
                if (jezos != NULL)
                {
                    fputs(historycommand, fptr2);
                }
            }
            // write the last command onto the file
            fputs(tempcommand, fptr2);
            fclose(fptr);
            fclose(fptr2);

            // delete the original history file and rename the temp file
            remove(historypath);
            rename(temphistorypath, historypath);
        }
    }
    return;
}

void history_read(char **words)
{
    lli arg_count = 0;
    lli i = 1;
    lli arg = 0;

    while (words[i] != NULL)
    {
        arg_count++;
        i++;
    }

    // no arguments for history
    if (arg_count > 0)
    {
        printf("bash: history: Too many arguments\n");
        return;
    }

    arg = 10;

    lli history_count = 0;

    FILE *fptr = fopen(historypath, "r");
    if (fptr == NULL)
    {
        printf("history: 5: Error opening file\n");
        return;
    }
    // count number of commands in the history file
    while (fgets(historycommand, sizeof(historycommand), fptr) != NULL)
    {
        history_count++;
    }

    fclose(fptr);

    // if it is less than 10 commands, print all
    if (history_count - arg < 0)
    {
        FILE *fptr = fopen(historypath, "r");
        if (fptr == NULL)
        {
            printf("history: Error opening file\n");
            return;
        }
        while (fgets(historycommand, sizeof(historycommand), fptr) != NULL)
        {
            if (!strcmp(historycommand, "\n"))
            {
                continue;
            }
            printf("%s", historycommand);
        }
        fclose(fptr);
    }
    else
    {
        FILE *fptr = fopen(historypath, "r");
        if (fptr == NULL)
        {
            printf("history: Error opening file\n");
            return;
        }

        // only print the last 10 commands
        for (int i = 0; i < (history_count - arg); i++)
        {
            fgets(historycommand, sizeof(historycommand), fptr);
        }
        while (fgets(historycommand, sizeof(historycommand), fptr) != NULL)
        {
            if (!strcmp(historycommand, "\n"))
            {
                continue;
            }
            printf("%s", historycommand);
        }
        fclose(fptr);
    }
    return;
}