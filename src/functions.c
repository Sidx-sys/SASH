#include "headers.h"

// using the global variable from utils.c
extern char init_dir[MAX_LIMIT];

// Function to implement *cd*
int ChangeDirectory(char* path) {
    if (!strcmp(path, "~")) {
        chdir(init_dir);
        return 0;
    }
    ReversePathModifier(path);

    int r_value = chdir(path);
    if (r_value == -1)
        perror("Error");
    return r_value;
}

// a helper function and also used in *pwd*
void CurrentDirectory() {
    char cur_dir[MAX_LIMIT];
    getcwd(cur_dir, MAX_LIMIT);
    printf("%s\n", cur_dir);

    return;
}

// function to implement echo
void Echo(char* args[]) {
    for (int i = 1; args[i] != NULL; i++)
        printf("%s ", args[i]);
    printf("\n");

    return;
}

// hepler function to implement ls -l and ls -la
void Permissions(char* Name) {
    char perm[11];
    struct stat st;
    if (stat(Name, &st) == 0) {
        mode_t permission = st.st_mode;
        perm[0] = (permission & S_IFDIR) ? 'd' : '-';
        perm[1] = (permission & S_IRUSR) ? 'r' : '-';
        perm[2] = (permission & S_IWUSR) ? 'w' : '-';
        perm[3] = (permission & S_IXUSR) ? 'x' : '-';
        perm[4] = (permission & S_IRGRP) ? 'r' : '-';
        perm[5] = (permission & S_IWGRP) ? 'w' : '-';
        perm[6] = (permission & S_IXGRP) ? 'x' : '-';
        perm[7] = (permission & S_IROTH) ? 'r' : '-';
        perm[8] = (permission & S_IWOTH) ? 'w' : '-';
        perm[9] = (permission & S_IXOTH) ? 'x' : '-';
        perm[10] = '\0';
        printf("%s ", perm);
    } else {
        perror("Error");
    }
    return;
}

// hepler function to implement ls -l and ls -la => group
void User(char* Name, char* return_arr, int len) {
    struct stat st;
    if (stat(Name, &st) == 0) {
        if (len == -1) {
            struct passwd* pw = getpwuid(st.st_uid);
            strcpy(return_arr, pw->pw_name);
        } else {
            printf("%s ", return_arr);
            int lim = len - strlen(return_arr);
            for (int i = 0; i < lim; i++)
                printf(" ");
        }
    } else {
        perror("Error");
    }
    return;
}

// hepler function to implement ls -l and ls -la => group
void Group(char* Name, char* return_arr, int len) {
    struct stat st;
    if (stat(Name, &st) == 0) {
        if (len == -1) {
            struct group* gr = getgrgid(st.st_gid);
            strcpy(return_arr, gr->gr_name);
        } else {
            printf("%s ", return_arr);
            int lim = len - strlen(return_arr);
            for (int i = 0; i < lim; i++)
                printf(" ");
        }
    } else {
        perror("Error");
    }
    return;
}

// hepler function to implement ls -l and ls -la => no. of hardlinks of a file/folder
void Links(char* Name, char* return_arr, int len) {
    struct stat st;
    if (stat(Name, &st) == 0) {
        if (len == -1) {
            char result[NUM_LIMIT];
            sprintf(result, "%ld", st.st_nlink);
            strcpy(return_arr, result);
        } else {
            int lim = len - strlen(return_arr);
            for (int i = 0; i < lim; i++)
                printf(" ");
            printf("%s ", return_arr);
        }
    } else {
        perror("Error");
    }
    return;
}

// hepler function to implement ls -l and ls -la => size in bytes
void Size(char* Name, char* return_arr, int len) {
    struct stat st;
    if (stat(Name, &st) == 0) {
        if (len == -1) {
            char result[NUM_LIMIT];
            sprintf(result, "%ld", st.st_size);
            strcpy(return_arr, result);
        } else {
            int lim = len - strlen(return_arr);
            for (int i = 0; i < lim; i++)
                printf(" ");
            printf("%s ", return_arr);
        }
    } else {
        perror("Error");
    }
    return;
}

// hepler function to implement ls -l and ls -la => last access time
void LastAccessTime(char* Name) {
    struct stat st;
    if (stat(Name, &st) == 0) {
        char dtmy[S_LIMIT];
        strcpy(dtmy, asctime(localtime(&(st.st_ctime))));
        dtmy[strlen(dtmy) - 1] = '\0';
        printf("%s ", dtmy);
    } else
        perror("Error");
    return;
}

// helper function to support (~) in prompt
void PathModifier(char* cur_dir) {
    int len_init = strlen(init_dir);
    char ext_name[S_LIMIT];
    // if the init_dir is present in prompt -> replace by (~)
    if (!strncmp(cur_dir, init_dir, len_init)) {
        strcpy(ext_name, cur_dir + len_init);
        strcpy(cur_dir, "~");
        strcat(cur_dir, ext_name);
    }
    return;
}

// helper function to support (~) in CL input
void ReversePathModifier(char* cur_dir) {
    // if (~) is present in the path, replace by the init_dir
    if (cur_dir[0] == '~') {
        char dir_name[S_LIMIT];
        strcpy(dir_name, cur_dir + 1);
        strcpy(cur_dir, init_dir);
        strcat(cur_dir, dir_name);
    }
    return;
}

int max(int a, int b) {
    if (a > b)
        return a;
    return b;
}

int Total(char* name) {
    struct stat st;
    if (stat(name, &st) == 0) {
        return st.st_blocks;
    } else {
        perror("Error");
    }
}

int Write_Redirect(int* fd, char* write_file) {
    *fd = open(write_file, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (*fd == -1) {
        printf("Error: file couldn't be created\n");
        return -1;
    }
    int save_stdout = dup(STDOUT_FILENO);
    if (save_stdout < 0) {
        perror("dup");
        exit(EXIT_FAILURE);
    }
    int return_val = dup2(*fd, STDOUT_FILENO);
    if (return_val < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    return save_stdout;
}

int Append_Redirect(int* fd, char* write_file) {
    *fd = open(write_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (*fd == -1) {
        printf("Error: file couldn't be created\n");
        return -1;
    }
    int save_stdout = dup(STDOUT_FILENO);
    if (save_stdout < 0) {
        perror("dup");
        exit(EXIT_FAILURE);
    }
    int return_val = dup2(*fd, STDOUT_FILENO);
    if (return_val < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    return save_stdout;
}

int Read_Redirect(int* fd, char* read_file) {
    *fd = open(read_file, O_RDONLY);
    if (*fd == -1) {
        printf("Error: file couldn't be created\n");
        return -1;
    }
    int save_stdin = dup(STDIN_FILENO);
    if (save_stdin < 0) {
        perror("dup");
        exit(EXIT_FAILURE);
    }
    int return_val = dup2(*fd, STDIN_FILENO);
    if (return_val < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    return save_stdin;
}

void Return_To_STDIN(int save_stdin, int fd) {
    if (fd > 0) {
        close(fd);
        int return_val = dup2(save_stdin, STDIN_FILENO);
        if (return_val < 0) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        return;
    }
}

void Return_To_STDOUT(int save_stdout, int fd) {
    if (fd > 0) {
        close(fd);
        int return_val = dup2(save_stdout, STDOUT_FILENO);
        if (return_val < 0) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        return;
    }
}