#ifndef _HEADER_H_
#define _HEADER_H_

#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <dirent.h>
#include <math.h>
#include <grp.h>
#include <sys/time.h>
#include <signal.h>
#include <termios.h>
#include <ctype.h>

#define SIZE 1000

#define COMMAND_LIMIT 100
#define MAX_COMMAND_LEN 500
#define MAX_WORD_LIMIT 100
#define MAX_POOL_SIZE 1000

#define GRN "\e[0;32m"
#define RED "\e[0;31m"
#define WHT "\e[0;34m"
#define RESET "\e[0m"

typedef long long int lli;

extern char homedir[SIZE];
extern char prevdir[SIZE];
extern char hostname[SIZE];
extern char currdir[SIZE];
extern char promptdir[SIZE];
extern char input[SIZE * 10];
extern char tempdir[SIZE];
extern char fileloc[SIZE];
extern char timebuffer[SIZE];
extern char discoverprev[SIZE];
extern char historycommand[SIZE * 10];
extern char lasthistorycommand[SIZE * 10];
extern char tempcommand[SIZE * 10];
extern char commandcheckercom[SIZE];
extern char promptstring[SIZE * 10];
extern char historypath[SIZE * 3];
extern char temphistorypath[SIZE * 3];
extern char promptstringdup[SIZE * 10];

extern double timeflag;
extern double timeelapsed;
extern lli children_count;
extern lli book;

struct process_info
{
    pid_t pid;
    char name[SIZE];
    int job_num;
};

struct process_info fg_process;

typedef void handler(int, siginfo_t *, void *);

void child_handler(int sig, siginfo_t *info, void *vp);
void ctrlc_handler(int sig, siginfo_t *info, void *vp);
void ctrlz_handler(int sig, siginfo_t *info, void *vp);

handler *install_signal(int signum, handler *handler);

struct process_info process_ledger[MAX_POOL_SIZE];

void newline();

void gethomedir();

void prompt();

void commandchecker(char *command, int bg, int fg);

void foreground(char **words);

void background(char **words);

void parseampersand(char *command);

void removespaces(char *command);

void parseinput(char *input);

void shell_main();

void pwd();

void echo();

void cd(char **words, lli count);

void ls(char **words);

lli count_digit(lli n);

void discover(char **words);

void history_read(char **words);

void history_write(char **words);

void initpool();

void killchildprocesses();

int ioredir(char **words);
void ioreset(int flag1, int flag2, int flag3, int stdinog, int stdoutog);
void debugpointer();

void jobs(char **words);
void sig_func_assignment(char **words);
void fg_function(char **words);
void bg_function(char **words);
void pipe_func(char *command);
void pinfo(char **words);
void inputmodify();
void enableRawMode();
void disableRawMode();
void die(const char *s);
void tabhandler(char *input);
void prefix(struct dirent **entries, int indexarray[], int filecount, char *input, int index);
#endif