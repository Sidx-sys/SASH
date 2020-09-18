#include "headers.h"

// using the global variable from utils.c
extern char init_dir[MAX_LIMIT];

// structure to store the background processes' name and pid
struct procName {
    char name[S_LIMIT];
    int pid;
};
// max possible no. of process running at a time
int pCounter = 0;
struct procName pName[S_LIMIT];

// Funtion to add a bakground process into process list
void AddProcess(char* name, int pid) {
    strcpy(pName[pCounter].name, name);
    pName[pCounter].pid = pid;
    pCounter++;
}

// Funtion to run a process
void Run_FG_BG(char* input) {
    // args for execvp
    char* args[S_LIMIT] = {NULL};
    // keeping a copy of command in case there is a "&" in the input
    char bg_input[MAX_CMD_LIMIT], modified_name[MID_LIMIT];
    strcpy(bg_input, input);

    // tokeninzing and forming args
    int num_args = 0;
    char* token = strtok(input, " ");
    while (token != NULL) {
        args[num_args] = token;
        num_args++;
        token = strtok(NULL, " ");
    }

    // checking if the executable path (args[i] has ~) to account for paths relative to ~
    for (int i = 0; i < num_args; i++) {
        char modified_path[2 * MAX_LIMIT];
        if (args[i][0] == '~') {
            strcpy(modified_name, args[i] + 1);
            sprintf(modified_path, "%s%s", init_dir, modified_name);
            args[i] = modified_path;
        }
    }

    // checking if the last part of args is "&" for background processes
    if (!strcmp(args[num_args - 1], "&")) {
        // have to run the process in background

        // removing the & from args
        args[num_args - 1] = NULL;

        // creating a child process
        pid_t pid = fork();
        if (pid < 0) {
            printf("Error while forking...\n");
        } else if (pid == 0) {
            // changing the process group so that process runs in the background
            setpgid(0, 0);
            execvp(args[0], args);

            // Error message to be printed incase the command is invalid
            printf("Command not found: %s\n", args[0]);  // !! not great error presentation
            exit(1);
        } else {
            // adding the process to keep record
            AddProcess(args[0], pid);
        }
    } else {
        // run in foreground
        pid_t pid = fork();
        if (pid < 0) {
            printf("Error while forking...\n");
        } else if (pid == 0) {
            execvp(args[0], args);
            printf("Command not found: %s\n", args[0]);  // process exits abnormally
            exit(1);
        }
        int status;
        // parent has to wait for the child process to finish
        wait(&status);
    }

    return;
}

// Function to implement the pinfo functionality
void Pinfo(int pid, int w_redirect, char* write_file) {
    char file_name[MAX_LIMIT];

    // pid given to input will be shell's or some other process
    // open the proc file of given pid
    sprintf(file_name, "/proc/%d/stat", pid);
    int f_size = MAX_LIMIT * 2, counter = 1, vsize;
    char stats[f_size + 1];
    char status[3];

    // open the stat file for the pid
    int stat_file = open(file_name, O_RDONLY);
    if (stat_file < 0) {
        perror("Error");
        return;
    }
    // read from the stat file for the size and status
    read(stat_file, stats, f_size);
    char* token = strtok(stats, " ");
    while (token != NULL) {
        if (counter == 3) {
            strcpy(status, token);
        } else if (counter == 23)
            vsize = atoi(token);
        counter++;
        token = strtok(NULL, " ");
    }
    close(stat_file);

    // for executable path derive the symlink from the exe executable of the given pid
    sprintf(file_name, "/proc/%d/exe", pid);
    char path[MAX_LIMIT];
    // get the symlink
    realpath(file_name, path);
    // edit the path if init_dir is present in it
    PathModifier(path);

    // checking for possible redirection
    int fd = 0, save_stdout;
    if (w_redirect == 1) {
        save_stdout = Write_Redirect(&fd, write_file);
    } else if (w_redirect == 2) {
        save_stdout = Append_Redirect(&fd, write_file);
    }

    // print the required info
    printf("pid -- %d\n", pid);
    printf("Process Status -- %s\n", status);
    printf("Memory -- %d\n", vsize);  // conversion not really required
    printf("Executable Path -- %s\n", path);

    // making the fd of STDOUT to 1 again if it has changed
    Return_To_STDOUT(save_stdout, fd);

    return;
}

// Function to handle the SIGHLD signal
void handler() {
    pid_t pid;
    int status, found = 0;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // String to get the name of process with PID = pid
        char progName[MID_LIMIT];

        // finding the Process name for the given programn
        for (int i = 0; i < pCounter; i++) {
            if (pName[i].pid == pid) {
                strcpy(progName, pName[i].name);
                pName[i].pid = -2;
                found = 1;
                break;
            }
        }
        // To prevent running for foreground processes => found
        if (found) {
            // print the required info
            char msg[MAX_LIMIT];
            if (status == 0)
                sprintf(msg, "%s with pid %d exited normally\n", progName, pid);
            else
                sprintf(msg, "%s with pid %d exited abnormally\n", progName, pid);

            write(STDERR, msg, strlen(msg));  // printed in STDERR
        }
    }

    return;
}