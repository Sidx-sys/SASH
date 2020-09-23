#include "headers.h"

char prompt_string[MAX_LIMIT];

void SigIntHandler(int signum) {
    // print the prompt string again
    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, prompt_string, strlen(prompt_string));
    return;
}

int main() {
    struct sigaction signalAction;
    memset(&signalAction, 0, sizeof(signalAction));
    signalAction.sa_handler = SigIntHandler;
    signalAction.sa_flags = SA_RESTART;
    sigemptyset(&signalAction.sa_mask);
    // to handle SIGINT
    sigaction(SIGINT, &signalAction, NULL);

    // initializing the terminal
    printf("\033[0;31mStarting Up Shell...\033[0m\n");

    // Prompt display
    Prompt();
    int shell = 1;
    shell = CLInput();
    // Handling the exit of background child processes
    signal(SIGCHLD, SIGCHLD_handler);

    // To keep the shell working until exited
    while (shell != -1) {
        Prompt();
        shell = CLInput();
    }
}
