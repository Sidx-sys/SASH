#include "headers.h"

// string to store the directory from where shell is spawned (~)
char init_dir[MAX_LIMIT];
int init = 1;

// Function to print prompt
void Prompt() {
    struct passwd* passwd;
    char prompt[MAX_LIMIT];

    // retrieving user and system name
    passwd = getpwuid(getuid());
    strcpy(prompt, passwd->pw_name);
    char sysName[MAX_LIMIT];
    gethostname(sysName, MAX_LIMIT);
    strcat(prompt, "@");
    strcat(prompt, sysName);

    char cur_dir[MAX_LIMIT];
    CurrentDirectory(cur_dir);
    if (init) {
        CurrentDirectory(init_dir);
        init = 0;
    }

    // function to support (~)
    PathModifier(cur_dir);

    // Prompt display with ANSI color code
    printf("\033[1;32m%s:\033[0m \033[1;36m%s\033[0m \033[1;32m>>\033[0m ", prompt, cur_dir);
}

// Function to take input from shell
int CLInput() {
    int return_val;
    // strings to store the command and properly spaced command
    char input[MAX_CMD_LIMIT], normalized_input[MAX_CMD_LIMIT];
    //Array of String pointers to store the different commands in the input
    char* cmd[MAX_LIMIT];
    // taking the command from shell
    scanf("%[^\n]%*c", input);
    // preprocessing the command (removing additional spaces)
    NormalizeInput(input, normalized_input);

    // Basic case handling of no inputs given
    int INV_semicolons = 1;
    for (int i = 0; i < MAX_CMD_LIMIT - 1; i++) {
        if (normalized_input[i] == '\0')
            break;
        else if (normalized_input[i] != ';' && normalized_input[i] != ' ')
            INV_semicolons = 0;
        // To check if only semicolons is given as input
    }
    if (INV_semicolons)
        printf("Invalid Input, ; is not a command\n");

    // separating the different commands
    char* token = strtok(normalized_input, ";");
    int num_cmd = 0;
    while (token != NULL) {
        cmd[num_cmd] = token;
        num_cmd++;
        token = strtok(NULL, ";");
    }

    // executing each command recieved in an input
    for (int i = 0; i < num_cmd; i++) {
        return_val = exec(cmd[i]);
        // checking for the exit command
        if (return_val == -1)
            return -1;
    }

    return 0;
}

int exec(char* cmd) {
    // two strings to contain two copies of command
    char command[MAX_CMD_LIMIT];
    strcpy(command, cmd);
    // one copy to be tokenized for command identification another to send for execution
    char* token = strtok(cmd, " ");

    // checking for various commands
    if (!strcmp(token, "exit")) {
        printf("\033[0;35mBye!\033[0m\n");
        return -1;
    } else if (!strcmp(token, "cd")) {
        token = strtok(NULL, " ");
        if (token == NULL)
            ChangeDirectory("~");
        else
            ChangeDirectory(token);
    } else if (!strcmp(token, "echo")) {
        Echo(command);
    } else if (!strcmp(token, "ls")) {
        FlagParser(command);
    } else if (!strcmp(token, "pwd")) {
        char cur_dir[MAX_LIMIT];
        CurrentDirectory(cur_dir);
        printf("%s\n", cur_dir);
    } else if (!strcmp(token, "clear")) {
        printf("\033[2J");
        printf("\033[H");
    } else if (!strcmp(token, "pinfo")) {
        token = strtok(NULL, " ");
        if (token == NULL) {
            int pid = getpid();
            Pinfo(pid);
        } else {
            int pid = atoi(token);
            Pinfo(pid);
        }
    } else {
        Run_FG_BG(command);
    }

    return 0;
}

// Funtion to even out the irregular spaces in a CL input
void NormalizeInput(char* input, char* fine_input) {
    int space_flag = 1, MAX_LEN = strlen(input), j = 0;
    char normalized_input[MAX_LEN + 1];

    for (int i = 0; i < MAX_LEN; i++) {
        if (input[i] == '\t') {
            input[i] = ' ';
        }
    }

    for (int i = 0; i < MAX_LEN; i++) {
        if (input[i] == ' ' && space_flag == 0) {
            normalized_input[j] = ' ';
            space_flag = 1;
            j++;
        } else if (input[i] != ' ' && input[i] != ';') {
            // extra case so that there is no space infront of a ";"
            normalized_input[j] = (char)input[i];
            j++;
            space_flag = 0;
        } else if (input[i] == ';') {
            normalized_input[j] = (char)input[i];
            j++;
            space_flag = 1;
        }
    }
    if (normalized_input[j - 1] == ' ')
        normalized_input[j - 1] = '\0';
    normalized_input[j] = '\0';
    // passing the refined input
    strcpy(fine_input, normalized_input);

    return;
}
