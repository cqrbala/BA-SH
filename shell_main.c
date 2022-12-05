//#ifndef HEADER_H
#include "header.h"

void shell_main()
{
    // write(1, promptstring, strlen(promptstring));
    printf("%s", promptstring);
    fflush(0);
    // if (!fgets(input, sizeof(input), stdin))
    // {
    //     printf("error\n");
    //     return;
    // }
    // if (strcmp(input, "\n") != 0)
    // {
    //     parseinput(input);
    // }
    inputmodify();
    prompt();
}
