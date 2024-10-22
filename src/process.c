#include "headers.h"

//ing to store the prompt string
extern char prompt_string[];

// using the global variable from utils.c
extern char init_dir[MAX_LIMIT];
extern int prev_stat;

// flag to keep check if there is any foreground process running
extern int fg_running;

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
void Run_FG(char* args[]) {
    // run in foreground
    pid_t pid = fork();
    if (pid < 0) {
        printf("Error while forking...\n");
    } else if (pid == 0) {
        execvp(args[0], args);
        printf("Command not found: %s\n", args[0]);  // process exits abnormally
        prev_stat = 0;
        exit(1);
    }
    // parent has to wait for the child process to finish
    int status;
    // foreground process is running
    fg_running = 1;

    //  waiting the process to terminate or stop
    int res = waitpid(pid, &status, WUNTRACED);
    if (res > 0) {
        // cheching if ctrl + z was pressed or not
        if (WIFSTOPPED(status)) {
            AddProcess(args[0], pid);
        }
    }

    if (status > 0)
        prev_stat = 0;

    return;
}

void Run_BG(char* args[]) {
    // have to run the process in background

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
        prev_stat = 0;
        exit(1);
    } else {
        // adding the process to keep record
        AddProcess(args[0], pid);
    }
}

// Function to implement the pinfo functionality
void Pinfo(int pid) {
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
        prev_stat = 0;
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

    // print the required info
    printf("pid -- %d\n", pid);
    printf("Process Status -- %s\n", status);
    printf("Memory -- %d\n", vsize);  // conversion not really required
    printf("Executable Path -- %s\n", path);

    return;
}

int DeleteProcess(char* progName, int pid) {
    int found = 0;

    // finding the Process name for the given program
    for (int i = 0; i < pCounter; i++) {
        if (pName[i].pid == pid) {
            strcpy(progName, pName[i].name);
            found = 1;
        } else if (found) {
            if (i < pCounter) {
                strcpy(pName[i - 1].name, pName[i].name);
                pName[i - 1].pid = pName[i].pid;
            }
            if (i == pCounter - 1) {
                pName[i].pid = -2;
            }
        }
    }

    if (found)
        pCounter--;

    return found;
}

// Function to handle the SIGHLD signal
void SIGCHLD_handler() {
    pid_t pid;
    int status, found = 0;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // String to get the name of process with PID = pid
        char progName[MID_LIMIT];

        // deleting the process (if it exists)
        found = DeleteProcess(progName, pid);

        // To prevent running for foreground processes => found
        if (found) {
            // print the required info
            char msg[MAX_LIMIT];
            if (status == 0)
                sprintf(msg, "%s with pid %d exited normally\n", progName, pid);
            else {
                sprintf(msg, "%s with pid %d exited abnormally\n", progName, pid);
                prev_stat = 0;
            }
            write(STDERR, msg, strlen(msg));  // printed in STDERR
        }
    }

    return;
}

void Jobs() {
    if (pCounter == 0)
        printf("No running background process\n");

    char file_name[MAX_LIMIT];
    for (int i = 0; i < pCounter; i++) {
        sprintf(file_name, "/proc/%d/stat", pName[i].pid);
        char status[NUM_LIMIT], stats[MAX_LIMIT];

        // open the stat file for the status
        int stat_file = open(file_name, O_RDONLY);
        if (stat_file < 0) {
            perror("Error");
            prev_stat = 0;
            return;
        }
        read(stat_file, stats, MAX_LIMIT);
        char* token = strtok(stats, " ");
        for (int i = 0; i < 2; i++)
            token = strtok(NULL, " ");

        // be sure of what to store
        if (*token == 'T')
            strcpy(status, "Stopped");
        else
            strcpy(status, "Running");

        printf("[%d] %s %s [%d]\n", i + 1, status, pName[i].name, pName[i].pid);
    }

    return;
}

int IsNumber(char* arg) {
    int len = strlen(arg);
    int valid = 1;
    for (int i = 0; i < len; i++) {
        if ((int)(arg[i] - '0') < 0 || (int)(arg[i] - '0') > 9) {
            valid = 0;
            break;
        }
    }
    return valid;
}

void Kjob(char* args[]) {
    // error handling related to arguments
    if (args[1] == NULL || args[2] == NULL || args[3] != NULL) {
        printf("Kjob: wrong input format: kjob <job number> <signal number>\n");
        prev_stat = 0;
        return;
    }

    // extracting the pid for the job number
    int p_no = atoi(args[1]) - 1, sig_no = atoi(args[2]);

    // error checking for process no.
    if (p_no >= pCounter) {
        printf("kjob: invalid job number\n");
        prev_stat = 0;
        return;
    }
    int pid = pName[atoi(args[1]) - 1].pid;

    // send signal to the process

    if (kill(pid, sig_no) < 0) {
        perror("kill");
        prev_stat = 0;
    }

    return;
}

void Overkill() {
    // looping through all the active background processes and sending the SIGKILL signal
    for (int i = 0; i < pCounter; i++) {
        if (kill(pName[i].pid, 9) < 0)
            perror("kill");
    }

    printf("Killed all background processes\n");

    return;
}

void Fg(char* args[]) {
    // error handling
    if (args[2] != NULL) {
        printf("fg: wrong input format -- fg <job number>\n");
        prev_stat = 0;
        return;
    }

    int valid = IsNumber(args[1]);
    if (!valid) {
        printf("fg: invalid job number\n");
        prev_stat = 0;
        return;
    }

    // getting process group of the shell
    pid_t gpid_shell = getpgid(0), pid_shell = getpid();

    // obtaining the pgid of the process to bring to foreground
    int job_no = atoi(args[1]) - 1;
    // error checking
    if (job_no >= pCounter) {
        printf("fg: invalid job number\n");
        prev_stat = 0;
        return;
    }

    // if process found, bring to fg and remove from jobs list
    char* p_name = pName[job_no].name;
    pid_t pid_bg = pName[job_no].pid;
    pid_t gpid_bg = getpgid(pid_bg);
    char temp[MAX_LIMIT];
    // remove from jobs list
    DeleteProcess(temp, pid_bg);
    // a process is running in foreground
    fg_running = 1;

    int status;

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    // changing the process group to send the SIGCONT signal to
    if (tcsetpgrp(0, gpid_bg) < 0) {
        perror("fg");
        prev_stat = 0;
        return;
    }

    if (kill(pid_bg, SIGCONT) < 0) {
        prev_stat = 0;
        perror("SIGCONT");
    }

    int res = waitpid(pid_bg, &status, WUNTRACED);
    if (res > 0) {
        // cheching if ctrl + z was pressed or not
        if (WIFSTOPPED(status)) {
            AddProcess(p_name, pid_bg);
        }
    }

    // when foreground process ends give control back to shell
    if (tcsetpgrp(0, gpid_shell) < 0) {
        perror("fg");
        prev_stat = 0;
        exit(0);
    }

    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    return;
}

void Bg(char* args[]) {
    // error handling
    if (args[2] != NULL) {
        printf("bg: wrong input format -- fg <job number>\n");
        prev_stat = 0;
        return;
    }

    int valid = IsNumber(args[1]);
    if (!valid) {
        printf("bg: invalid job number\n");
        prev_stat = 0;
        return;
    }

    // obtaining the pgid of the process to bring to foreground
    int job_no = atoi(args[1]) - 1;
    // error checking
    if (job_no >= pCounter) {
        printf("bg: invalid job number\n");
        prev_stat = 0;
        return;
    }

    // if process found, obtain the group and process id
    pid_t pid_bg = pName[job_no].pid;

    if (kill(pid_bg, SIGCONT) < 0) {
        perror("SIGCONT");
        prev_stat = 0;
    }

    return;
}