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
    getcwd(cur_dir, MAX_LIMIT);
    if (init) {
        getcwd(init_dir, MAX_LIMIT);
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
    if (INV_semicolons) {
        printf("Invalid Input, ; is not a command\n");
        return 0;
    }

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
    // flags and attributes for redirection
    char* args[S_LIMIT + 5] = {NULL};
    int r_redirect = 0, w_redirect = 0, num_args = 0, r_file = S_LIMIT, w_file = S_LIMIT;
    char *read_file = NULL, *write_file = NULL;

    // two strings to contain two copies of command
    char command[MAX_CMD_LIMIT];
    strcpy(command, cmd);
    // one copy to be tokenized for command identification another to send for execution
    char* token = strtok(cmd, " ");

    // checking the input for *redirections*
    while (token != NULL) {
        args[num_args] = token;
        if (!strcmp(token, ">")) {
            w_redirect = 1;  // assuming that *>* is not used arbitrarily
            w_file = num_args + 1;
        } else if (!strcmp(token, ">>")) {
            w_redirect = 2;  // assuming that *>* is not used arbitrarily
            w_file = num_args + 1;
        } else if (!strcmp(token, "<")) {
            r_redirect = 1;
            r_file = num_args + 1;
        }
        num_args++;
        token = strtok(NULL, " ");
    }

    // checking if the executable path (args[i] has ~) to account for paths relative to ~
    char modified_name[MID_LIMIT];
    for (int i = 0; i < num_args; i++) {
        char modified_path[2 * MAX_LIMIT];
        if (args[i][0] == '~') {
            strcpy(modified_name, args[i] + 1);
            sprintf(modified_path, "%s%s", init_dir, modified_name);
            args[i] = modified_path;
        }
    }

    // technically blocking arguments after main command post flag and file fetching
    read_file = args[r_file];
    write_file = args[w_file];
    args[w_file - 1] = NULL;
    args[r_file - 1] = NULL;

    // to add support to redirection (input/output)
    int fd_out = 0, save_stdout, fd_in = 0, save_stdin;
    if (w_redirect == 1) {
        save_stdout = Write_Redirect(&fd_out, write_file);
    } else if (w_redirect == 2) {
        save_stdout = Append_Redirect(&fd_out, write_file);
    }
    if (r_redirect == 1)
        save_stdin = Read_Redirect(&fd_in, read_file);

    // checking for various commands
    if (!strcmp(args[0], "exit")) {
        printf("\033[0;35mBye!\033[0m\n");
        return -1;
    } else if (!strcmp(args[0], "cd")) {
        char* token_cmd = strtok(command, " ");
        token_cmd = strtok(NULL, " ");
        if (token_cmd == NULL)
            ChangeDirectory("~");
        else if (!strcmp(token_cmd, "<") || !strcmp(token_cmd, ">") || !strcmp(token_cmd, ">>")) {
            ChangeDirectory("~");
            token_cmd = strtok(NULL, " ");
            int fd = open(token_cmd, O_RDONLY | O_CREAT, 0644);
            if (fd == -1)
                printf("Error: file couldn't be created\n");
        } else
            ChangeDirectory(token_cmd);
    } else if (!strcmp(args[0], "echo")) {
        Echo(args, w_redirect, write_file);
    } else if (!strcmp(args[0], "ls")) {
        FlagParser(args, w_redirect, write_file);
    } else if (!strcmp(args[0], "pwd")) {
        CurrentDirectory(w_redirect, write_file);
    } else if (!strcmp(args[0], "clear")) {
        printf("\033[2J");
        printf("\033[H");
    } else if (!strcmp(args[0], "pinfo")) {
        char* token_cmd = strtok(command, " ");
        token_cmd = strtok(NULL, " ");
        if (token_cmd == NULL) {
            int pid = getpid();
            Pinfo(pid, 0, NULL);
        } else if (!strcmp(token_cmd, ">") || !strcmp(token_cmd, ">>")) {
            int pid = getpid();
            Pinfo(pid, w_redirect, write_file);
        } else {
            int pid = atoi(token_cmd);
            Pinfo(pid, w_redirect, write_file);
        }
    } else if (!strcmp(args[num_args - 1], "&")) {
        args[num_args - 1] = NULL;
        Run_BG(args, w_redirect, r_redirect, write_file, read_file);
    } else {
        Run_FG(args, w_redirect, r_redirect, write_file, read_file);
    }

    // making the fd of STDOUT to 1, STDIN to 0, if it has changed
    Return_To_STDOUT(save_stdout, fd_out);
    Return_To_STDIN(save_stdin, fd_in);

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
