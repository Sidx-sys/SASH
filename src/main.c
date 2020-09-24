#include "headers.h"

char prompt_string[MAX_LIMIT];

void SigIntHandler(int signum) {
    // print the prompt string again
    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, prompt_string, strlen(prompt_string));
    return;
}

void SigTstpHandler(int signum) {
    write(STDOUT_FILENO, "\n", 1);
    return;
}

int main() {
    struct sigaction sigintAction, sigtstpAction;
    memset(&sigintAction, 0, sizeof(sigintAction));
    sigintAction.sa_handler = SigIntHandler;
    sigintAction.sa_flags = SA_RESTART;
    sigemptyset(&sigintAction.sa_mask);
    // to handle SIGINT
    sigaction(SIGINT, &sigintAction, NULL);

    memset(&sigtstpAction, 0, sizeof(sigtstpAction));
    sigtstpAction.sa_handler = SigTstpHandler;
    sigtstpAction.sa_flags = SA_RESTART;
    sigemptyset(&sigtstpAction.sa_mask);
    // to handle SIGTSTP
    sigaction(SIGTSTP, &sigtstpAction, NULL);

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
