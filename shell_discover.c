#include "header.h"

// checks for the target file
void checkforfile(char *path, char *originalpath, char *filename)
{
    struct dirent **dir_entries;
    int s = scandir(path, &dir_entries, NULL, NULL);

    for (int i = 0; i < s; i++)
    {
        if (!strcmp(dir_entries[i]->d_name, ".") || !strcmp(dir_entries[i]->d_name, ".."))
        {
            continue;
        }
        char structitempath[SIZE];
        char printingpath[SIZE];
        strcpy(structitempath, path);
        strcat(structitempath, "/");
        strcat(structitempath, dir_entries[i]->d_name);

        DIR *diritem = opendir(structitempath);
        if (!diritem)
        {
            if (errno == ENOTDIR)
            {
                strcpy(printingpath, originalpath);
                strcat(printingpath, dir_entries[i]->d_name);
                // if file we're looking for is found, print it's relative path and set global variable to 1
                if (!strcmp(dir_entries[i]->d_name, filename))
                {
                    printf("%s\n", printingpath);
                    book = 1;
                }
                continue;
            }
        }
        else
        {
            strcpy(printingpath, originalpath);
            strcat(printingpath, dir_entries[i]->d_name);
            strcat(printingpath, "/");
            // call recursively for the directory
            checkforfile(structitempath, printingpath, filename);
        }
    }
    return;
}

// similar to print structure function except this has conditions
void printwithcondition(char *path, char *originalpath, lli d, lli f)
{
    struct dirent **dir_entries;
    int s = scandir(path, &dir_entries, NULL, NULL);

    for (int i = 0; i < s; i++)
    {
        if (!strcmp(dir_entries[i]->d_name, ".") || !strcmp(dir_entries[i]->d_name, ".."))
        {
            continue;
        }
        char structitempath[SIZE];
        char printingpath[SIZE];
        strcpy(structitempath, path);
        strcat(structitempath, "/");
        strcat(structitempath, dir_entries[i]->d_name);

        DIR *diritem = opendir(structitempath);
        if (!diritem)
        {
            if (errno == ENOTDIR)
            {
                strcpy(printingpath, originalpath);
                strcat(printingpath, dir_entries[i]->d_name);
                // print file name only if the file flag is present
                if (f == 1)
                {
                    printf("%s\n", printingpath);
                }
                continue;
            }
        }
        else
        {
            strcpy(printingpath, originalpath);
            strcat(printingpath, dir_entries[i]->d_name);
            // print dir name only if the dir flag is given
            if (d == 1)
            {
                printf("%s\n", printingpath);
            }
            strcat(printingpath, "/");
            printwithcondition(structitempath, printingpath, d, f);
        }
    }
    return;
}

void printstructure(char *path, char *originalpath)
{
    // get the items in the current directory using scandir
    struct dirent **dir_entries;
    int s = scandir(path, &dir_entries, NULL, NULL);

    // iterate through dir items
    for (int i = 0; i < s; i++)
    {
        // skip these files as they lead to infinite loops
        if (!strcmp(dir_entries[i]->d_name, ".") || !strcmp(dir_entries[i]->d_name, ".."))
        {
            continue;
        }
        // get the corresponding path
        char structitempath[SIZE];
        char printingpath[SIZE];
        strcpy(structitempath, path);
        strcat(structitempath, "/");
        strcat(structitempath, dir_entries[i]->d_name);

        DIR *diritem = opendir(structitempath);
        if (!diritem)
        {
            // if the dir item is a file, print it
            if (errno == ENOTDIR)
            {
                strcpy(printingpath, originalpath);
                strcat(printingpath, dir_entries[i]->d_name);
                printf("%s\n", printingpath);
                continue;
            }
        }
        else
        {
            strcpy(printingpath, originalpath);
            strcat(printingpath, dir_entries[i]->d_name);
            printf("%s\n", printingpath);
            strcat(printingpath, "/");

            // if the item is a dir, recursively call the function on that dir
            printstructure(structitempath, printingpath);
        }
    }
    return;
}

void discover(char **words)
{
    lli d = 0;
    lli f = 0;
    lli fileflag = 0;
    lli dirflag = 0;
    lli i = 1;
    char filename[SIZE];

    while (words[i] != NULL)
    {
        // check for d and f flags
        if (words[i][0] == '-')
        {
            if (words[i][1] == 'd')
            {
                d = 1;
            }
            if (words[i][1] == 'f')
            {
                f = 1;
            }
        }
        // if file encountered, save the name without the quotes
        else if (words[i][0] == '"')
        {
            fileflag = 1;
            lli j = 0;
            for (lli z = 1; z < strlen(words[i]) - 1; z++)
            {
                filename[j++] = words[i][z];
            }
            filename[j] = '\0';
        }
        // without the - or "", the arg must be a dir so set the flag
        else
        {
            dirflag = 1;
        }
        i++;
    }

    // if no dir arg, assume currdir
    if (dirflag == 0)
    {
        strcpy(tempdir, currdir);
    }
    else
    {
        // if dir arg is there, replace ~ with home dir
        if (!strcmp(words[1], "~"))
        {
            strcpy(tempdir, homedir);
        }
        // check if the given dir is valid and get it's path
        else
        {
            strcpy(discoverprev, currdir);
            int y = chdir(words[1]);
            if (y == -1)
            {
                printf("discover: '%s': No such file or directory\n", words[1]);
                return;
            }
            getcwd(tempdir, sizeof(tempdir));
            chdir(discoverprev);
        }
    }
    // open dir and get the pointer to the stream
    DIR *discoverdir = opendir(tempdir);
    if (!discoverdir)
    {
        printf("discover: '%s': No such file or directory\n", words[1]);
        return;
    }

    // print dir structure under these cases
    if (((d == 1) && (f == 1) && (fileflag == 0)) || ((d == 0) && (f == 0) && (fileflag == 0)))
    {
        if (!strcmp(tempdir, currdir))
        {
            printf(".\n");
            printstructure(tempdir, "./");
        }
        else
        {
            char argument2[SIZE];
            strcpy(argument2, words[1]);
            strcat(argument2, "/");
            printf("%s\n", words[1]);
            printstructure(tempdir, argument2);
        }
    }
    // if d flag is only present, then call the function with appr parameters
    else if ((d == 1) && (f == 0) && (fileflag == 0))
    {
        if (!strcmp(tempdir, currdir))
        {
            printf(".\n");
            printwithcondition(tempdir, "./", 1, 0);
        }
        else
        {
            char argument2[SIZE];
            strcpy(argument2, words[1]);
            strcat(argument2, "/");
            printf("%s\n", words[1]);
            printwithcondition(tempdir, argument2, 1, 0);
        }
    }
    // if only f flag is present
    else if ((d == 0) && (f == 1) && (fileflag == 0))
    {
        if (!strcmp(tempdir, currdir))
        {
            printwithcondition(tempdir, "./", 0, 1);
        }
        else
        {
            char argument2[SIZE];
            strcpy(argument2, words[1]);
            strcat(argument2, "/");
            printwithcondition(tempdir, argument2, 0, 1);
        }
    }
    // check for file function if file flag is present
    else if (fileflag == 1)
    {
        if (!strcmp(tempdir, currdir))
        {
            checkforfile(tempdir, "./", filename);
            if (book == 0)
            {
                printf("discover: '%s': No such file\n", filename);
            }
            book = 0;
        }
        else
        {
            char argument2[SIZE];
            strcpy(argument2, words[1]);
            strcat(argument2, "/");
            checkforfile(tempdir, argument2, filename);
            if (book == 0)
            {
                printf("discover: '%s': No such file\n", filename);
            }
            book = 0;
        }
    }
    return;
}