//#ifndef HEADER_H
#include "header.h"

char homedir[SIZE];
char prevdir[SIZE];
char hostname[SIZE];
char currdir[SIZE];
char promptdir[SIZE];
char input[SIZE * 10];
char tempdir[SIZE];
char fileloc[SIZE];
char timebuffer[SIZE];
char discoverprev[SIZE];
char historycommand[SIZE * 10];
char lasthistorycommand[SIZE * 10];
char tempcommand[SIZE * 10];
char commandcheckercom[SIZE];
char promptstring[SIZE * 10];
char promptstringdup[SIZE * 10];
char historypath[SIZE * 3];
char temphistorypath[SIZE * 3];

double timeflag;
double timeelapsed;
lli children_count;
lli book;

int main()
{
    fg_process.job_num = 0;
    strcpy(fg_process.name, "");
    fg_process.pid = -1;
    book = 0;
    gethomedir();
    initpool();
    install_signal(SIGCHLD, child_handler);
    install_signal(SIGINT, ctrlc_handler);
    install_signal(SIGTSTP, ctrlz_handler);
    prompt();
    children_count = 0;

    while (1)
    {
        shell_main();
    }
}
