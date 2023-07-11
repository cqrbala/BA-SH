## Running the shell

Run `make` to compile the code, followed by `./a.out` to run the shell, from the directory the files related to the program are stored.

## Assumptions & Important notes

> It has been assumed that the shell is only invoked from the directory where the program files are kept and this is a necessary condition for the **history** function to work.

## Major Files

### header.h

Contains function declarations, global variables and header files used.

### main.c

Background process pool initialization and signal handling functions have been called before invoking the `shell_main()` routine that takes input and prints the prompt string.

### general_functions.c

Functions for reading the input in raw mode, parsing it while acocunting for: pipes `pipe_func()`, ampersand `parseampersand()`, cleaning the spaces `removespaces()`, and executing the command using the `commandchecker()` function.

## Shell functionalities

### 1. IO REDIRECTION

IO Redirection has been implemented in `commandchecker()` function in the `general_functions.c` file where based on the redirection symbols present in the input command, the STDIN and STDOUT file pointers are redirected using the **dup/dup2** commands to the respective files.

Upon command execution, it is reset to what it was originally pointing to.

### 2. COMMAND PIPIELINES

`pipe_func()` function in `general_functions.c` iterates the input command for valid pipes.
In the presence of valid pipes, the following happens for every command split by a pipe:

-> create a pipe

-> fork a child process

-> redirect STDIN and STDOUT based on the command ( read end of the first command is STDIN and write end of the last command is STDOUT )

-> continue with the normal execution cycle of the program.

-> close the necessary pipes and set it apprioriately for the next command in the child and parent processes of the fork.

### 3. IO REDIRECTION WITH COMMAND PIPELINES

works automatically due to the modularity of the code for the 2 separate parts.

### 4. USER DEFINED COMMANDS

* JOBS
`shell_jobs.c` contains the routine for jobs where based on the flags, we iterate through the background process pool while checking a process's status in /proc/pid/stat folder and printing it appropriately.

* SIG
`shell_sig.c` contains the routine for iterating through the pool of bg processes to find the one with the given job number and sending the given signal.

* FG
`shell_fg.c` contains the routine for iterating through the pool of bg processe to find the one with the given job number, ignoring signals to deny accesss of terminal while we move it to foreground, continuing execution of it and waiting till it terminates.

* BG
`shell_bg.c` contains the routine for iterating through the pool of bg processes to find the one with the given job number and sending **SIGCONT** signal to continue execution of it.

### 5. SIGNAL HANDLING
* CTRL+Z
`ctrlz_handler` function in `shell_background.c` takes care of signal handling for this signal.

* CTRL + C
`ctrlc_handler` function present in `shell_background.c` responds to this signal.

* CTRL + D
`inputmodify( )` function in `general_functions.c` kills all child processes and exits on this input.

### 6. AUTOCOMPLETION

`inputmodify()` function in `general_functions.c` enables rawmode and contains if conditions for handling different types of control input.

Upon a tab, `tabhandler()` function in the same file is called and it gets a list of files/directories in the current directory you're working on that have common characters as your incomplete word, which is passed to `prefix()` function to find the largest common prefix.

In case the list of files/directories is empty, we return and if it is 1, we autofill that file/directory.

* To accomodate for '/' & ' ' after folder or file, stat is used and appropriate flags are checked to see if it is a directory or file.

### 7. BUILTIN COMMANDS

cd, echo, pwd and ls have been implemented in their respective files.

### 8. FOREGROUND AND BACKGROUND processes

This functionality has been implemented using the execvp system call mainly.

### 9. DISCOVER 

Command allows you to iteratively check for existing files in a directory.

### 10. PROCESS INFO

pinfo - command tells the status of the currently running processes, be it foreground or background.
