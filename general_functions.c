#include "header.h"

// kills the current background processes
void killchildprocesses()
{
    for (lli i = 0; i < MAX_POOL_SIZE; i++)
    {
        if (process_ledger[i].pid != -1)
        {
            printf("[%d] %d : terminated\n", process_ledger[i].job_num, process_ledger[i].pid);
            kill(process_ledger[i].pid, SIGKILL);
            children_count--;
            process_ledger[i].job_num = 0;
            strcpy(process_ledger[i].name, "");
            process_ledger[i].pid = -1;
        }
    }
    return;
}

// initializes the pool for background processes
void initpool()
{
    for (lli i = 0; i < MAX_POOL_SIZE; i++)
    {
        process_ledger[i].pid = -1;
        strcpy(process_ledger[i].name, "");
        process_ledger[i].job_num = 0;
    }
}

lli count_digit(lli n)
{
    lli count = 0;
    if (n == 0)
    {
        return 1;
    }
    while (n != 0)
    {
        n = n / 10;
        count++;
    }
    return count;
}

void newline()
{
    printf("\n");
}

// gets the invoking dir when the shell starts
void gethomedir()
{
    getcwd(homedir, sizeof(homedir));
    getcwd(prevdir, sizeof(prevdir));
}

// functions used to display the prompt
void prompt()
{
    struct passwd *data;
    data = getpwuid(getuid());

    gethostname(hostname, sizeof(hostname));
    getcwd(currdir, sizeof(currdir));

    // if curr dir length is less than print it as a whole
    if (strlen(currdir) < strlen(homedir))
    {
        if (timeflag == 1)
        {
            sprintf(tempcommand, "took %.0lfs", timeelapsed);
            sprintf(promptstring, "<%s@%s:%s%s>", data->pw_name, hostname, currdir, tempcommand);
            timeflag = 0;
            return;
        }
        sprintf(promptstring, "<%s@%s:%s>", data->pw_name, hostname, currdir);
        return;
    }

    // if the currdir is the homedir, print with ~ and exit
    if (strcmp(currdir, homedir) == 0)
    {
        if (timeflag == 1)
        {
            sprintf(tempcommand, "took %.0lfs", timeelapsed);
            sprintf(promptstring, "<%s@%s:~%s>", data->pw_name, hostname, tempcommand);
            timeflag = 0;
            return;
        }
        sprintf(promptstring, "<%s@%s:~>", data->pw_name, hostname);
        return;
    }

    // if curr dir length is more or equal, check if it is a subdir of the home dir
    int flag = 0;
    for (lli i = 0; i < strlen(homedir); i++)
    {
        if (homedir[i] != currdir[i])
        {
            flag = 1;
            break;
        }
    }

    // if it is not a subdir, print as it is
    if (flag == 1)
    {
        if (timeflag == 1)
        {
            sprintf(tempcommand, "took %.0lfs", timeelapsed);
            sprintf(promptstring, "<%s@%s:%s%s>", data->pw_name, hostname, currdir, tempcommand);
            timeflag = 0;
            return;
        }
        sprintf(promptstring, "<%s@%s:%s>", data->pw_name, hostname, currdir);
        return;
    }

    // if it is a subdir, ~/ and add the subdir
    if (flag == 0)
    {
        int j = 1;
        promptdir[0] = '~';
        // promptdir[1] = '/';

        for (lli i = 0; i < strlen(currdir); i++)
        {
            if (i < strlen(homedir))
            {
                // printf("%c ", currdir[i]);
                continue;
            }
            // printf("\nafter\n");
            promptdir[j++] = currdir[i];
            // printf("%c ", promptdir[j - 1]);
        }
        promptdir[j] = '\0';
        if (timeflag == 1)
        {
            sprintf(tempcommand, "took %.0lfs", timeelapsed);
            sprintf(promptstring, "<%s@%s:%s%s>", data->pw_name, hostname, promptdir, tempcommand);
            timeflag = 0;
            return;
        }
        sprintf(promptstring, "<%s@%s:%s>", data->pw_name, hostname, promptdir);
        // printf("\n%s\n%s\n", promptstring, currdir);
        return;
    }
    return;
}

void die(const char *s)
{
    // perror(s);
    // exit(1);
    return;
}

struct termios orig_termios;

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}
void prefix(struct dirent **entries, int indexarray[], int filecount, char *input, int index)
{
    int flag = 0;
    int shortestlength = 0;
    char *newinput = (char *)malloc(6969 * sizeof(char));
    char incomplete_word[200];
    for (int i = 0; i < filecount; i++)
    {
        if (flag == 0)
        {
            shortestlength = strlen(entries[indexarray[i]]->d_name);
            flag = 1;
        }
        else
        {
            shortestlength = shortestlength < strlen(entries[indexarray[i]]->d_name) ? shortestlength : strlen(entries[indexarray[i]]->d_name);
        }
    }

    int common = 0;
    int itr = 0;

    for (int i = 1; i <= shortestlength; i++)
    {
        for (int j = 1; j < filecount; j++)
        {
            if (strncmp(entries[indexarray[j - 1]]->d_name, entries[indexarray[j]]->d_name, i))
            {
                for (int i = 0; i < common; i++)
                {
                    incomplete_word[itr] = entries[indexarray[0]]->d_name[i];
                    itr++;
                }
                incomplete_word[itr] = '\0';
                struct stat curritem;
                int y = lstat(incomplete_word, &curritem);
                if (S_IFDIR & curritem.st_mode)
                {
                    strcat(incomplete_word, "/");
                }
                else
                {
                    if (S_IFREG & curritem.st_mode)
                    {
                        strcat(incomplete_word, " ");
                    }
                }
                int ptr = 0;
                int ptr2 = 0;
                for (ptr = 0; ptr < index; ptr++)
                {
                    newinput[ptr] = input[ptr];
                }
                for (ptr2 = 0; ptr2 < strlen(incomplete_word); ptr2++)
                {
                    newinput[ptr + ptr2] = incomplete_word[ptr2];
                }
                newinput[ptr + ptr2] = '\0';
                strcpy(input, newinput);
                free(newinput);
                return;
            }
        }
        common++;
    }
    free(newinput);
    return;
}

void tabhandler(char *input)
{
    int index;
    int flag = 0;
    for (int i = strlen(input) - 1; i >= 0; i--)
    {
        if (input[i] == 32)
        {
            flag = 1;
            index = i + 1;
            break;
        }
    }
    if (flag == 0)
    {
        index = 0;
    }

    if (index == strlen(input))
    {
        strcpy(tempdir, currdir);
        struct dirent **entries;
        int filecount = scandir(tempdir, &entries, NULL, alphasort);
        int index_array[filecount];
        int normal_filecount = 0;

        for (int i = 0; i < filecount; i++)
        {
            if (entries[i]->d_name[0] != '.')
            {
                index_array[normal_filecount] = i;
                normal_filecount++;
            }
        }
        if (normal_filecount == 0)
        {
            return;
        }
        else if (normal_filecount == 1)
        {
            strcat(input, entries[index_array[0]]->d_name);
            struct stat curritem;
            int y = lstat(entries[index_array[0]]->d_name, &curritem);
            if (S_IFDIR & curritem.st_mode)
            {
                strcat(entries[index_array[0]]->d_name, "/");
            }
            else
            {
                if (S_IFREG & curritem.st_mode)
                {
                    strcat(entries[index_array[0]]->d_name, " ");
                }
            }
            return;
        }
        else
        {
            for (int i = 0; i < normal_filecount; i++)
            {
                struct stat curritem;
                int y = lstat(entries[index_array[i]]->d_name, &curritem);
                if (S_IFDIR & curritem.st_mode)
                {
                    printf("%s/\n", entries[index_array[i]]->d_name);
                }
                else
                {
                    printf("%s\n", entries[index_array[i]]->d_name);
                }
            }
            prefix(entries, index_array, normal_filecount, input, index);
            return;
        }
    }
    else
    {
        char *newinput = (char *)malloc(6969 * sizeof(char));
        char incomplete_word[200];
        int x = 0;
        for (int i = index; i < strlen(input); i++)
        {
            incomplete_word[x] = input[i];
            x++;
        }
        incomplete_word[x] = '\0';

        strcpy(tempdir, currdir);
        struct dirent **entries;
        int filecount = scandir(tempdir, &entries, NULL, alphasort);
        int index_array[filecount];
        int normal_filecount = 0;

        for (int i = 0; i < filecount; i++)
        {
            if (entries[i]->d_name[0] != '.')
            {
                if (!strncmp(incomplete_word, entries[i]->d_name, strlen(incomplete_word)))
                {
                    index_array[normal_filecount] = i;
                    normal_filecount++;
                }
            }
        }

        if (normal_filecount == 0)
        {
            free(newinput);
            return;
        }
        else if (normal_filecount == 1)
        {
            if (!strncmp(incomplete_word, entries[index_array[0]]->d_name, strlen(incomplete_word)))
            {
                strcpy(incomplete_word, entries[index_array[0]]->d_name);
                struct stat curritem;
                int y = lstat(incomplete_word, &curritem);
                if (S_IFDIR & curritem.st_mode)
                {
                    strcat(incomplete_word, "/");
                }
                else
                {
                    if (S_IFREG & curritem.st_mode)
                    {
                        strcat(incomplete_word, " ");
                    }
                }
            }
        }
        else
        {
            for (int i = 0; i < normal_filecount; i++)
            {
                if (!strncmp(incomplete_word, entries[index_array[i]]->d_name, strlen(incomplete_word)))
                {
                    struct stat curritem;
                    int y = lstat(entries[index_array[i]]->d_name, &curritem);
                    if (S_IFDIR & curritem.st_mode)
                    {
                        printf("%s/\n", entries[index_array[i]]->d_name);
                    }
                    else
                    {
                        printf("%s\n", entries[index_array[i]]->d_name);
                    }
                }
            }
            prefix(entries, index_array, normal_filecount, input, index);
            free(newinput);
            return;
        }
        int ptr = 0;
        int ptr2 = 0;
        for (ptr = 0; ptr < index; ptr++)
        {
            newinput[ptr] = input[ptr];
        }
        for (ptr2 = 0; ptr2 < strlen(incomplete_word); ptr2++)
        {
            newinput[ptr + ptr2] = incomplete_word[ptr2];
        }
        newinput[ptr + ptr2] = '\0';
        strcpy(input, newinput);
        free(newinput);
        return;
    }
}

void inputmodify()
{
    char *input = (char *)malloc(6969 * sizeof(char));
    char c;

    setbuf(stdout, NULL);
    enableRawMode();
    memset(input, '\0', 100);
    int ptr = 0;
    char promptstringlocal[SIZE * 10];
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (iscntrl(c))
        {
            if (c == 10)
            {
                input[ptr++] = c;
                printf("\n");
                break;
            }
            else if (c == 127)
            { // backspace
                if (ptr > 0)
                {
                    ptr--;
                    input[ptr] = '\0';
                    printf("\b \b");
                }
            }
            else if (c == 9)
            {
                printf("\n");
                int prev = strlen(input);
                tabhandler(input);
                sprintf(promptstringlocal, "%s%s", promptstring, input);
                printf("\n%s", promptstringlocal);
                ptr = ptr + (strlen(input) - prev);
            }
            else if (c == 4)
            {
                killchildprocesses();
                printf("\n");
                exit(0);
            }
        }
        else
        {
            input[ptr++] = c;
            printf("%c", c);
        }
    }
    disableRawMode();
    parseinput(input);
    free(input);
}

// creates pointers to every word in a command and calls the corresponding function
void commandchecker(char *command, int bg, int fg)
{
    char *words[MAX_WORD_LIMIT + 10] = {NULL};
    char *wordsafterioredir[MAX_WORD_LIMIT + 10] = {NULL};
    char *singleword;
    singleword = strtok(command, " ");
    lli wordcount = 0;

    while (singleword != NULL)
    {
        words[wordcount++] = singleword;
        singleword = strtok(NULL, " ");
    }

    int input_redir = 0;
    int output_redir1 = 0;
    int output_redir2 = 0;
    int stdinog, stdoutog;
    int flag1 = 0;
    int flag2 = 0;
    int flag3 = 0;
    char *input_redirfile, *output_redir1file, *output_redir2file;
    int afterioredir = 0;
    for (lli i = 0; i < wordcount; i++)
    {
        if (!strcmp("<", words[i]))
        {
            if (i + 1 < wordcount)
            {
                input_redir = open(words[i + 1], O_RDONLY);
                if (input_redir < 0)
                {
                    printf("bash: Input file not found\n");
                    close(input_redir);
                    return;
                }
                close(input_redir);
                input_redirfile = words[i + 1];
                i++;
                flag1 = 1;
            }
            else
            {
                printf("bash: Invalid input\n");
                return;
            }
        }
        else if (!strcmp(">", words[i]))
        {
            if (i + 1 < wordcount)
            {
                output_redir1 = open(words[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                close(output_redir1);
                output_redir1file = words[i + 1];
                i++;
                flag2 = 1;
            }
            else
            {
                printf("bash: Invalid input\n");
                return;
            }
        }
        else if (!strcmp(">>", words[i]))
        {
            if (i + 1 < wordcount)
            {
                output_redir2 = open(words[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                close(output_redir2);
                output_redir2file = words[i + 1];
                i++;
                flag3 = 1;
            }
            else
            {
                printf("bash: Invalid input\n");
                return;
            }
        }
        else
        {
            wordsafterioredir[afterioredir] = words[i];
            afterioredir++;
        }
    }

    if (flag1 == 1)
    {
        stdinog = dup(0);
        if (stdinog == -1)
        {
            printf("bash: error in duping stdin\n");
            return;
        }
        input_redir = open(input_redirfile, O_RDONLY);
        int check = dup2(input_redir, 0);
        if (check == -1)
        {
            printf("bash: error in redirecting stdin\n");
            return;
        }
        close(input_redir);
    }
    if (flag2 == 1)
    {
        stdoutog = dup(1);
        if (stdoutog == -1)
        {
            printf("bash: error in duping stdout\n");
            return;
        }
        output_redir1 = open(output_redir1file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_redir1 < 0)
        {
            printf("bash: error opening file\n");
            return;
        }
        int check = dup2(output_redir1, 1);
        if (check == -1)
        {
            printf("bash: error in redirecting stdout\n");
            return;
        }
        close(output_redir1);
    }
    if (flag3 == 1)
    {
        stdoutog = dup(1);
        if (stdoutog == -1)
        {
            printf("bash: error in duping stdout\n");
            return;
        }
        output_redir2 = open(output_redir2file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (output_redir2 < 0)
        {
            printf("bash: error opening file\n");
            return;
        }
        int check = dup2(output_redir2, 1);
        if (check == -1)
        {
            printf("bash: error in redirecting stdout\n");
            return;
        }
        close(output_redir2);
    }

    if (wordsafterioredir[0] != NULL)
    {
        if (!strcmp(wordsafterioredir[0], "cd"))
        {
            history_write(wordsafterioredir);
            if (afterioredir == 1)
            {
                chdir(homedir);
                return;
            }
            cd(wordsafterioredir, afterioredir);
        }
        else if (!strcmp(wordsafterioredir[0], "pwd"))
        {
            history_write(wordsafterioredir);
            pwd();
        }
        else if (!strcmp(wordsafterioredir[0], "echo"))
        {
            history_write(wordsafterioredir);
            echo(wordsafterioredir);
        }
        else if (!strcmp(wordsafterioredir[0], "ls"))
        {
            history_write(wordsafterioredir);
            ls(wordsafterioredir);
        }
        else if (!strcmp(wordsafterioredir[0], "discover"))
        {
            history_write(wordsafterioredir);
            discover(wordsafterioredir);
        }
        else if (!strcmp(wordsafterioredir[0], "history"))
        {
            history_write(wordsafterioredir);
            history_read(wordsafterioredir);
        }
        else if (!strcmp(wordsafterioredir[0], "fg"))
        {
            history_write(wordsafterioredir);
            fg_function(wordsafterioredir);
        }
        else if (!strcmp(wordsafterioredir[0], "bg"))
        {
            history_write(wordsafterioredir);
            bg_function(wordsafterioredir);
        }
        else if (!strcmp(wordsafterioredir[0], "sig"))
        {
            history_write(wordsafterioredir);
            sig_func_assignment(wordsafterioredir);
        }
        else if (!strcmp(wordsafterioredir[0], "jobs"))
        {
            history_write(wordsafterioredir);
            jobs(wordsafterioredir);
        }
        else if (!strcmp(wordsafterioredir[0], "pinfo"))
        {
            history_write(wordsafterioredir);
            pinfo(wordsafterioredir);
        }
        else if (!strcmp(wordsafterioredir[0], "quit"))
        {
            history_write(wordsafterioredir);
            killchildprocesses();
            if (flag1 == 1)
            {
                dup2(stdinog, 0);
            }
            if (flag2 == 1 || flag3 == 1)
            {
                dup2(stdoutog, 1);
            }
            exit(1);
        }
        else
        {
            history_write(wordsafterioredir);
            if (bg)
            {
                background(wordsafterioredir);
            }
            else if (fg)
            {
                foreground(wordsafterioredir);
            }
        }
    }
    if (flag1 == 1)
    {
        dup2(stdinog, 0);
    }
    if (flag2 == 1 || flag3 == 1)
    {
        dup2(stdoutog, 1);
    }
    return;
}

void pipe_func(char *finalcommand)
{
    int pipe_count = 0;
    int word_count = 0;
    char command[MAX_COMMAND_LEN] = "";
    strcpy(command, finalcommand);

    char *word = strtok(command, " ");

    while (word != NULL)
    {
        word_count++;
        if (!strcmp(word, "|"))
        {
            if (word_count == 1)
            {
                printf("bash: Invalid piping\n");
                return;
            }
            else
            {
                pipe_count++;
                word = strtok(NULL, " ");
                if (!word)
                {
                    printf("bash: Invalid piping\n");
                    return;
                }
            }
        }
        word = strtok(NULL, " ");
    }

    if (pipe_count == 0)
    {
        parseampersand(finalcommand);
    }
    else
    {
        strcpy(command, finalcommand);

        char *split_by_pipes[MAX_WORD_LIMIT] = {NULL};
        char *word = strtok(command, "|");
        int x = 0;
        while (word != NULL)
        {
            split_by_pipes[x] = word;
            x++;
            word = strtok(NULL, "|");
        }

        int present_input = dup(STDIN_FILENO);
        pid_t id;
        for (int i = 0; i <= pipe_count; i++)
        {
            char commandtopass[MAX_COMMAND_LEN] = "";
            char *single = strtok(split_by_pipes[i], " ");
            while (single != NULL)
            {
                strcat(commandtopass, single);
                strcat(commandtopass, " ");
                single = strtok(NULL, " ");
            }

            int pipes[2];
            if (pipe(pipes))
            {
                printf("problem in creating pipe\n");
            }

            id = fork();
            if (id < 0)
            {
                printf("bash: Error in creating child process\n");
                return;
            }

            if (id == 0)
            {
                if (i < pipe_count)
                {
                    dup2(pipes[1], STDOUT_FILENO);
                }
                dup2(present_input, STDIN_FILENO);
                close(pipes[0]);
                parseampersand(commandtopass);
                exit(0);
            }
            else
            {
                waitpid(id, NULL, 0);
                close(pipes[1]);
                present_input = pipes[0];
            }
        }
    }
    return;
}

// parses the input based on &
void parseampersand(char *command)
{
    char *bg[200] = {NULL};
    char *fg[200] = {NULL};
    lli bg_head = 0;
    lli fg_head = 0;
    lli c = 0;
    for (lli check = 0; check < strlen(command); check++)
    {
        if (command[check] == '&')
        {
            c++;
        }
    }
    char *tokenizedcommand;
    tokenizedcommand = strtok(command, "&");
    while (tokenizedcommand != NULL)
    {
        if (c)
        {
            // if there is & left, this must be a bg process so store it's pointer appropriately.
            if (commandcheckercom[0] == ' ')
            {
                bg[bg_head] = &tokenizedcommand[1];
                bg_head++;
            }
            else
            {
                bg[bg_head] = tokenizedcommand;
                bg_head++;
            }
        }
        else
        {
            // no & left, this must be a fg process so store it's pointer appropriately.
            if (strcmp(commandcheckercom, " "))
            {
                if (commandcheckercom[0] == ' ')
                {
                    fg[fg_head] = &tokenizedcommand[1];
                    fg_head++;
                }
                else
                {
                    fg[fg_head] = tokenizedcommand;
                    fg_head++;
                }
            }
        }
        c--;
        tokenizedcommand = strtok(NULL, "&");
    }

    // call fg and bg processses

    for (lli x = 0; x < bg_head; x++)
    {
        commandchecker(bg[x], 1, 0);
    }
    for (lli x = 0; x < fg_head; x++)
    {
        commandchecker(fg[x], 0, 1);
    }
}

// cleans the input string based on spaces
void removespaces(char *command)
{
    char *word;
    char finalcommand[MAX_COMMAND_LEN] = "";
    word = strtok(command, " ");

    while (word != NULL)
    {
        strcat(finalcommand, word);
        strcat(finalcommand, " ");
        word = strtok(NULL, " ");
    }

    pipe_func(finalcommand);
}

// parses the input based on tabs and ;
void parseinput(char *input)
{
    for (lli i = 0; i < strlen(input); i++)
    {
        // replace tabs with space
        if (input[i] == '\t')
        {
            input[i] = ' ';
        }
        // replace new line with ; as it is a new command anyways
        else if (input[i] == '\n')
        {
            input[i] = ';';
        }
    }

    // tokenized based on ; and remove spaces to clean it
    char *commands[COMMAND_LIMIT + 10] = {NULL};
    lli count = 0;
    commands[count] = strtok(input, ";");
    while (commands[count] != NULL)
    {
        if (count >= COMMAND_LIMIT)
        {
            break;
        }
        commands[++count] = strtok(NULL, ";");
    }

    for (lli i = 0; i < count; i++)
    {
        removespaces(commands[i]);
    }
}
