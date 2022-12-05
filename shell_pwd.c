//#ifndef HEADER_H
#include "header.h"
// #endif
void pwd()
{
    getcwd(currdir, sizeof(currdir));
    printf("%s", currdir);
    newline();
}
