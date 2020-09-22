#include "headers.h"

// Function to parse the command "ls" with attributes and directories
void FlagParser(char* args[]) {
    // array of strings to store directories
    char dirs[NUM_LIMIT][S_LIMIT];
    // flags related to the ls command
    int l_flag = 0, a_flag = 0, dir_freq = 0;

    // checking the case when only ls is given as input
    if (args[1] == NULL) {
        Ls(".");
        return;
    }

    // processing the ls command
    int i = 1;
    while (args[i] != NULL) {
        if (!strcmp(args[i], "-a"))
            a_flag = 1;
        else if (!strcmp(args[i], "-l"))
            l_flag = 1;
        else if (!strcmp(args[i], "-la") || !strcmp(args[i], "-al"))
            a_flag = l_flag = 1;
        else {
            strcpy(dirs[dir_freq], args[i]);

            int len = strlen(args[i]);
            dirs[dir_freq][len] = '\0';

            ++dir_freq;
        }
        i++;
    }

    // case-wise execution of the command
    if (a_flag && l_flag) {
        // -l -a | -la | -al
        if (dir_freq > 1) {
            for (int i = 0; i < dir_freq; i++) {
                printf("%s:\n", dirs[i]);
                Ls_LA(dirs[i]);
                if (i != dir_freq - 1)
                    printf("\n");
            }
        } else if (dir_freq == 1) {
            Ls_LA(dirs[0]);
        } else {
            Ls_LA(".");
        }
    } else if (a_flag) {
        // -a
        if (dir_freq > 1) {
            for (int i = 0; i < dir_freq; i++) {
                printf("%s:\n", dirs[i]);
                Ls_A(dirs[i]);
                if (i != dir_freq - 1)
                    printf("\n");
            }
        } else if (dir_freq == 1) {
            Ls_A(dirs[0]);
        } else {
            Ls_A(".");
        }
    } else if (l_flag) {
        // -l
        if (dir_freq > 1) {
            for (int i = 0; i < dir_freq; i++) {
                printf("%s:\n", dirs[i]);
                Ls_L(dirs[i]);
                if (i != dir_freq - 1)
                    printf("\n");
            }
        } else if (dir_freq == 1) {
            Ls_L(dirs[0]);
        } else {
            Ls_L(".");
        }
    } else {
        if (dir_freq > 1) {
            for (int i = 0; i < dir_freq; i++) {
                printf("%s:\n", dirs[i]);
                Ls(dirs[i]);
                if (i != dir_freq - 1)
                    printf("\n");
            }
        } else {
            Ls(dirs[0]);
        }
    }

    return;
}

// Output function for ls
void Ls(char* path) {
    ReversePathModifier(path);

    struct dirent* de;
    DIR* dr = opendir(path);

    if (dr == NULL) {
        printf("Could not open directory, wrong Address/Name\n");
        return;
    }
    while ((de = readdir(dr)) != NULL) {
        if (de->d_name[0] != '.')
            printf("%s ", de->d_name);
    }
    printf("\n");
    return;
}

// Output function for ls -a
void Ls_A(char* path) {
    ReversePathModifier(path);

    struct dirent* de;
    DIR* dr = opendir(path);

    if (dr == NULL) {
        printf("Could not open directory, wrong Address/Name\n");
        return;
    }
    while ((de = readdir(dr)) != NULL) {
        printf("%s ", de->d_name);
    }
    printf("\n");
    return;
}

// Output function for ls -la | ls -a -l | ls -al
void Ls_LA(char* path) {
    ReversePathModifier(path);

    struct dirent* de;
    DIR* dr = opendir(path);

    if (dr == NULL) {
        printf("Could not open directory, wrong Address/Name\n");
        return;
    }
    // array to have the correct name
    char name[S_LIMIT];

    // array to store outputs for correct formatting
    char links[S_LIMIT][NUM_LIMIT];
    char user[S_LIMIT][NUM_LIMIT];
    char group[S_LIMIT][NUM_LIMIT];
    char size[S_LIMIT][NUM_LIMIT];
    // variables to store maximum lengths for formatting
    int f_counter = 0, max_links = 0, max_size = 0, max_user = 0, max_group = 0, total = 0;

    // Two loops => one to calculate the padding length, other to print
    while ((de = readdir(dr)) != NULL) {
        // to avoid errors where we need to explore a the childs of a directory that are not in
        // the current directory
        strcpy(name, path);
        strcat(name, "/");
        strcat(name, de->d_name);

        total += (Total(name) * 512);
        Links(name, links[f_counter], -1);
        max_links = max(max_links, strlen(links[f_counter]));

        User(name, user[f_counter], -1);
        max_user = max(max_user, strlen(user[f_counter]));

        Group(name, group[f_counter], -1);
        max_group = max(max_group, strlen(group[f_counter]));

        Size(name, size[f_counter], -1);
        max_size = max(max_size, strlen(size[f_counter]));
        f_counter++;
    }

    f_counter = 0;
    dr = opendir(path);
    printf("total %d\n", (total + 1023) / 1024);
    while ((de = readdir(dr)) != NULL) {
        // to avoid errors where we need to explore a the childs of a directory that are not in
        // the current directory
        strcpy(name, path);
        strcat(name, "/");
        strcat(name, de->d_name);

        Permissions(name);
        Links(name, links[f_counter], max_links);
        User(name, user[f_counter], max_user);
        Group(name, group[f_counter], max_group);
        Size(name, size[f_counter], max_size);
        LastAccessTime(name);
        printf("%s\n", de->d_name);
        f_counter++;
    }
    return;
}

// Output function for ls -l
void Ls_L(char* path) {
    ReversePathModifier(path);

    struct dirent* de;
    DIR* dr = opendir(path);

    if (dr == NULL) {
        printf("Could not open directory, wrong Address/Name\n");
        return;
    }
    // array to have the correct name
    char name[S_LIMIT];

    // array to store outputs for correct formatting
    char links[S_LIMIT][NUM_LIMIT];
    char user[S_LIMIT][NUM_LIMIT];
    char group[S_LIMIT][NUM_LIMIT];
    char size[S_LIMIT][NUM_LIMIT];
    // variables to store maximum lengths for formatting
    int f_counter = 0, max_links = 0, max_size = 0, max_user = 0, max_group = 0, total = 0;

    while ((de = readdir(dr)) != NULL) {
        // to avoid errors where we need to explore a the childs of a directory that are not in
        // the current directory
        if (de->d_name[0] != '.') {
            // condition for -l
            strcpy(name, path);
            strcat(name, "/");
            strcat(name, de->d_name);

            total += (Total(name) * 512 + 1023) / 1024;
            Links(name, links[f_counter], -1);
            max_links = max(max_links, strlen(links[f_counter]));

            User(name, user[f_counter], -1);
            max_user = max(max_user, strlen(user[f_counter]));

            Group(name, group[f_counter], -1);
            max_group = max(max_group, strlen(group[f_counter]));

            Size(name, size[f_counter], -1);
            max_size = max(max_size, strlen(size[f_counter]));
            f_counter++;
        }
    }

    f_counter = 0;
    dr = opendir(path);
    printf("total %d\n", total);
    while ((de = readdir(dr)) != NULL) {
        // to avoid errors where we need to explore a the childs of a directory that are not in
        // the current directory
        if (de->d_name[0] != '.') {
            // condition for -l
            strcpy(name, path);
            strcat(name, "/");
            strcat(name, de->d_name);

            Permissions(name);
            Links(name, links[f_counter], max_links);
            User(name, user[f_counter], max_user);
            Group(name, group[f_counter], max_group);
            Size(name, size[f_counter], max_size);
            LastAccessTime(name);
            printf("%s\n", de->d_name);
            f_counter++;
        }
    }
    return;
}
