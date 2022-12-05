//#ifndef HEADER_H
#include "header.h"

void echo(char **words)
{
    lli i = 1;
    // print every word & add newline
    while (words[i] != NULL)
    {
        printf("%s ", words[i]);
        i++;
    }
    newline();
}
