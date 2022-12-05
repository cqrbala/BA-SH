#include "header.h"

lli c1 = 10, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0;

// getting permissions and printing them
void Printperms(struct stat filetoprint)
{
    char permstring[11];

    if (S_ISDIR(filetoprint.st_mode))
    {
        permstring[0] = 'd';
    }
    else if (S_ISCHR(filetoprint.st_mode))
    {
        permstring[0] = 'c';
    }
    else if (S_ISBLK(filetoprint.st_mode))
    {
        permstring[0] = 'b';
    }
    else if (S_ISFIFO(filetoprint.st_mode))
    {
        permstring[0] = 'f';
    }
    else if (S_ISLNK(filetoprint.st_mode))
    {
        permstring[0] = 'l';
    }
    else if (S_ISSOCK(filetoprint.st_mode))
    {
        permstring[0] = 's';
    }
    else
    {
        permstring[0] = '-';
    }

    if (filetoprint.st_mode & S_IRUSR)
    {
        permstring[1] = 'r';
    }
    else
    {
        permstring[1] = '-';
    }

    if (filetoprint.st_mode & S_IWUSR)
    {
        permstring[2] = 'w';
    }
    else
    {
        permstring[2] = '-';
    }
    if (filetoprint.st_mode & S_IXUSR)
    {
        permstring[3] = 'x';
    }
    else
    {
        permstring[3] = '-';
    }

    if (filetoprint.st_mode & S_IRGRP)
    {
        permstring[4] = 'r';
    }
    else
    {
        permstring[4] = '-';
    }
    if (filetoprint.st_mode & S_IWGRP)
    {
        permstring[5] = 'w';
    }
    else
    {
        permstring[5] = '-';
    }
    if (filetoprint.st_mode & S_IXGRP)
    {
        permstring[6] = 'x';
    }
    else
    {
        permstring[6] = '-';
    }

    if (filetoprint.st_mode & S_IROTH)
    {
        permstring[7] = 'r';
    }
    else
    {
        permstring[7] = '-';
    }

    if (filetoprint.st_mode & S_IWOTH)
    {
        permstring[8] = 'w';
    }
    else
    {
        permstring[8] = '-';
    }

    if (filetoprint.st_mode & S_IXOTH)
    {
        permstring[9] = 'x';
    }
    else
    {
        permstring[9] = '-';
    }
    permstring[10] = '\0';
    for (int i = 0; i < 11; i++)
    {
        printf("%c", permstring[i]);
    }
    printf(" ");
    return;
}

void Printdetails(struct stat filetoprint, struct dirent **entries, int i)
{
    // align no. of hard links to the right
    for (lli i = count_digit(filetoprint.st_nlink); i < c2; i++)
    {
        printf(" ");
    }
    printf("%d ", filetoprint.st_nlink);

    // align the user to the left
    printf("%s ", getpwuid(filetoprint.st_uid)->pw_name);
    for (lli i = strlen(getpwuid(filetoprint.st_uid)->pw_name); i < c3; i++)
    {
        printf(" ");
    }

    // align the group to the left
    printf("%s ", getgrgid(filetoprint.st_gid)->gr_name);
    for (lli i = strlen(getgrgid(filetoprint.st_gid)->gr_name); i < c4; i++)
    {
        printf(" ");
    }

    // align the size to the right
    for (lli i = count_digit(filetoprint.st_size); i < c5; i++)
    {
        printf(" ");
    }
    printf("%lld ", filetoprint.st_size);

    // time of creation
    strftime(timebuffer, sizeof(timebuffer), "%b %d %H:%M ", localtime(&filetoprint.st_mtime));
    printf("%s", timebuffer);

    if (i == -1)
    {
        return;
    }

    // print file/dir name with appropriate colour

    if (S_IFDIR & filetoprint.st_mode)
    {
        printf("%s%s%s\n", RED, entries[i]->d_name, RESET);
        return;
    }
    if (S_IEXEC & filetoprint.st_mode)
    {
        printf("%s%s%s\n", GRN, entries[i]->d_name, RESET);
        return;
    }
    printf("%s%s%s\n", WHT, entries[i]->d_name, RESET);
    return;
}

void ls(char **words)
{
    lli arguments = 0;
    lli flags = 0;
    int l = 0;
    int a = 0;
    lli i = 1;
    while (words[i] != NULL)
    {
        arguments++;
        if (words[i][0] == '-')
        {
            // count the number of flags & set the flag var appropriately
            flags++;
            if (strchr(words[i], 'l') != NULL)
            {
                l = 1;
            }
            if (strchr(words[i], 'a') != NULL)
            {
                a = 1;
            }
        }
        i++;
    }

    int dir_count = arguments - flags;
    int spacedircount = dir_count;

    // if there is no dir argument, assume it to be the currdir
    if (!dir_count)
    {
        strcpy(words[0], ".");
    }

    for (lli x = 0; x <= arguments; x++)
    {
        // checking for directory
        if (words[x][0] != '-' && (strcmp(words[x], "ls")))
        {
            struct dirent **entries;
            lli total = 0;

            // if ~, replace it with the homedir
            if (!strcmp(words[x], "~"))
            {
                strcpy(tempdir, homedir);
            }

            // if ~/... replace ~ with home dir and concatenate the remaining path
            else if (words[x][0] == '~')
            {
                strcpy(tempdir, homedir);
                strcat(tempdir, &words[x][1]);
            }

            // assume everything else to be a subdir
            else
            {
                strcpy(tempdir, currdir);
                strcat(tempdir, "/");
                strcat(tempdir, words[x]);
            }

            DIR *dirpointer = opendir(tempdir);
            if (!dirpointer)
            {
                // if arg not directory
                if (errno == ENOTDIR)
                {
                    int flag = 0;
                    DIR *currdirpointer = opendir(currdir);
                    struct dirent *currdirentry = readdir(currdirpointer);

                    // check if the file is present in the currdir
                    while (currdirentry != NULL)
                    {
                        if (!strcmp(currdirentry->d_name, words[x]))
                        {
                            flag = 1;
                            break;
                        }
                        currdirentry = readdir(currdirpointer);
                    }
                    if (flag == 1)
                    {
                        // if file is present and l flag is given,print details
                        if (l)
                        {
                            struct stat filedetails;
                            strcpy(fileloc, tempdir);
                            int y = lstat(fileloc, &filedetails);
                            if (y == -1)
                            {
                                return;
                            }
                            Printperms(filedetails);
                            Printdetails(filedetails, entries, -1);

                            // print appropriate colour based on file type
                            if (S_IEXEC & filedetails.st_mode)
                            {
                                printf("%s%s%s\n", GRN, words[x], RESET);
                            }
                            else
                            {
                                printf("%s%s%s\n", WHT, words[x], RESET);
                            }
                        }
                        // if no flag given (-a doesn't matter), then print it with appropriate colour
                        else
                        {
                            struct stat filedetails;
                            strcpy(fileloc, tempdir);
                            int y = lstat(fileloc, &filedetails);
                            if (y == -1)
                            {
                                return;
                            }
                            if (S_IEXEC & filedetails.st_mode)
                            {
                                printf("%s%s%s\n", GRN, words[x], RESET);
                            }
                            else
                            {
                                printf("%s%s%s\n", WHT, words[x], RESET);
                            }
                        }
                    }
                    // if file is not present in currdir
                    else
                    {
                        printf("ls: cannot access '%s': No such file or directory\n", words[x]);
                        continue;
                    }
                }
                else
                {
                    printf("ls: cannot access '%s': No such file or directory\n", words[x]);
                    continue;
                }
                continue;
            }

            // get the files in the dir in alphabetical order
            int filecount = scandir(tempdir, &entries, NULL, alphasort);

            // iterate through files
            for (int i = 0; i < filecount; i++)
            {
                // if l flag present, get the max lengths for each of the columns to be displayed
                if (l == 1)
                {
                    strcpy(fileloc, tempdir);
                    strcat(fileloc, "/");
                    strcat(fileloc, entries[i]->d_name);

                    struct stat fileinfo;
                    int y = lstat(fileloc, &fileinfo);
                    if (y == -1)
                    {
                        return;
                    }
                    if (entries[i]->d_name[0] == '.')
                    {
                        // consider hidden files only if -a flag is there
                        if (a == 1)
                        {
                            // c2 = fmax(c2, count_digit(fileinfo.st_nlink));
                            c2 = c2 > count_digit(fileinfo.st_nlink) ? c2 : count_digit(fileinfo.st_nlink);
                            // c3 = fmax(c3, strlen(getpwuid(fileinfo.st_uid)->pw_name));
                            c3 = c3 > strlen(getpwuid(fileinfo.st_uid)->pw_name) ? c3 : strlen(getpwuid(fileinfo.st_uid)->pw_name);
                            // c4 = fmax(c4, strlen(getgrgid(fileinfo.st_gid)->gr_name));
                            c4 = c4 > strlen(getgrgid(fileinfo.st_gid)->gr_name) ? c4 : strlen(getgrgid(fileinfo.st_gid)->gr_name);
                            // c5 = fmax(c5, count_digit(fileinfo.st_size));
                            c5 = c5 > count_digit(fileinfo.st_size) ? c5 : count_digit(fileinfo.st_size);
                            total += (fileinfo.st_blocks * 512 + 1023) / 1024;
                        }
                    }
                    else
                    {
                        // c2 = fmax(c2, count_digit(fileinfo.st_nlink));
                        c2 = c2 > count_digit(fileinfo.st_nlink) ? c2 : count_digit(fileinfo.st_nlink);
                        // c3 = fmax(c3, strlen(getpwuid(fileinfo.st_uid)->pw_name));
                        c3 = c3 > strlen(getpwuid(fileinfo.st_uid)->pw_name) ? c3 : strlen(getpwuid(fileinfo.st_uid)->pw_name);
                        // c4 = fmax(c4, strlen(getgrgid(fileinfo.st_gid)->gr_name));
                        c4 = c4 > strlen(getgrgid(fileinfo.st_gid)->gr_name) ? c4 : strlen(getgrgid(fileinfo.st_gid)->gr_name);
                        // c5 = fmax(c5, count_digit(fileinfo.st_size));
                        c5 = c5 > count_digit(fileinfo.st_size) ? c5 : count_digit(fileinfo.st_size);
                        total += (fileinfo.st_blocks * 512 + 1023) / 1024;
                    }
                }
            }

            total = total * 2;
            closedir(dirpointer);

            if (dir_count > 1)
            {
                printf("%s:", words[x]);
                newline();
            }

            dirpointer = opendir(tempdir);
            if (!dirpointer)
            {
                printf("ls: %s: No such file or directory\n", tempdir);
                return;
            }

            if (l)
            {
                printf("total %lld\n", total);
                if (a)
                {
                    // if l & a flags present, print details of all files
                    for (int i = 0; i < filecount; i++)
                    {
                        strcpy(fileloc, tempdir);
                        strcat(fileloc, "/");
                        strcat(fileloc, entries[i]->d_name);
                        struct stat filetoprint;
                        int y = lstat(fileloc, &filetoprint);
                        if (y == -1)
                        {
                            return;
                        }
                        Printperms(filetoprint);
                        Printdetails(filetoprint, entries, i);
                    }
                    // printing appropriate spaces after every dir
                    if (spacedircount > 1)
                    {
                        newline();
                        spacedircount--;
                    }
                }
                // if -a is not there, ignore hidden files
                else
                {
                    for (int i = 0; i < filecount; i++)
                    {
                        if (entries[i]->d_name[0] != '.')
                        {
                            strcpy(fileloc, tempdir);
                            strcat(fileloc, "/");
                            strcat(fileloc, entries[i]->d_name);
                            struct stat filetoprint;
                            int y = lstat(fileloc, &filetoprint);
                            if (y == -1)
                            {
                                return;
                            }
                            Printperms(filetoprint);
                            Printdetails(filetoprint, entries, i);
                        }
                    }
                    if (spacedircount > 1)
                    {
                        newline();
                        spacedircount--;
                    }
                }
            }
            // if only a is present, print only the hidden files or directories
            else if (a)
            {
                for (int i = 0; i < filecount; i++)
                {
                    if (entries[i]->d_name[0] == '.')
                    {
                        strcpy(fileloc, tempdir);
                        strcat(fileloc, "/");
                        strcat(fileloc, entries[i]->d_name);
                        struct stat filetoprint;
                        int y = lstat(fileloc, &filetoprint);
                        if (y == -1)
                        {
                            return;
                        }
                        if (S_IFDIR & filetoprint.st_mode)
                        {
                            printf("%s%s%s\n", RED, entries[i]->d_name, RESET);
                            continue;
                        }
                        if (S_IEXEC & filetoprint.st_mode)
                        {
                            printf("%s%s%s\n", GRN, entries[i]->d_name, RESET);
                            continue;
                        }
                        printf("%s%s%s\n", WHT, entries[i]->d_name, RESET);
                    }
                }
                if (spacedircount > 1)
                {
                    newline();
                    spacedircount--;
                }
            }
            // if no flag is present, ignore hidden dir & files and print the rest
            else
            {
                for (int i = 0; i < filecount; i++)
                {
                    if (entries[i]->d_name[0] != '.')
                    {
                        strcpy(fileloc, tempdir);
                        strcat(fileloc, "/");
                        strcat(fileloc, entries[i]->d_name);
                        struct stat filetoprint;
                        int y = lstat(fileloc, &filetoprint);
                        if (y == -1)
                        {
                            return;
                        }
                        if (S_IFDIR & filetoprint.st_mode)
                        {
                            printf("%s%s%s\n", RED, entries[i]->d_name, RESET);
                            continue;
                        }
                        if (S_IEXEC & filetoprint.st_mode)
                        {
                            printf("%s%s%s\n", GRN, entries[i]->d_name, RESET);
                            continue;
                        }
                        printf("%s%s%s\n", WHT, entries[i]->d_name, RESET);
                    }
                }
                if (spacedircount > 1)
                {
                    newline();
                    spacedircount--;
                }
            }
            c1 = 0;
            c2 = 0;
            c3 = 0;
            c4 = 0;
            c5 = 0;
        }
    }
}