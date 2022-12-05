#include "header.h"

void cd(char **words, lli count)
{
    // invalid cd if arg count is > 2
    if (count > 2)
    {
        printf("bash: cd: too many arguments");
        newline();
        return;
    }

    strcpy(tempdir, prevdir);
    char *target = words[1];
    // if target is ~, change to homedir
    if (!strcmp(target, "~"))
    {
        strcpy(prevdir, currdir);
        chdir(homedir);
        return;
    }
    // if cd .. in homedir, change dir and print the home dir on terminal
    if (!strcmp(target, "..") && !strcmp(currdir, homedir))
    {
        strcpy(prevdir, currdir);
        chdir(target);
        printf("%s", homedir);
        newline();
        return;
    }
    // if the first character is ~, go to home dir, and then dir for the remaining path
    if (target[0] == '~')
    {
        strcpy(prevdir, currdir);
        chdir(homedir);
        int x = chdir(&target[2]);
        if (x == -1)
        {
            strcpy(prevdir, tempdir);
            chdir(currdir);
        }
        return;
    }
    // move to prev dir, when -
    if (!strcmp(target, "-"))
    {
        strcpy(prevdir, currdir);
        chdir(tempdir);
        return;
    }
    strcpy(prevdir, currdir);
    // chdir to target different from the above cases
    int x = chdir(target);
    if (x == -1)
    {
        printf("bash: cd: %s: No such file or directory\n", target);
        strcpy(prevdir, tempdir);
        chdir(currdir);
    }
    return;
}
