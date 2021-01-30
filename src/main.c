#include "headers.h"

char prompt_string[MAX_LIMIT];
int fg_running;

void SigIntHandler(int signum) {
    char prompt_w_exit[MAX_LIMIT + 100];

    if (fg_running) {
        write(STDOUT_FILENO, "\n", 1);
        fg_running = 0;
        return;
    }
    sprintf(prompt_w_exit, "\033[0;31m:'(\033[0m%s", prompt_string);

    // print the prompt string again
    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, prompt_w_exit, strlen(prompt_w_exit));
    return;
}

void SigTstpHandler(int signum) {
    char prompt_w_exit[MAX_LIMIT + 100];

    if (fg_running) {
        write(STDOUT_FILENO, "\n", 1);
        fg_running = 0;
        return;
    }

    sprintf(prompt_w_exit, "\033[0;31m:'(\033[0m%s", prompt_string);

    // print the prompt string again
    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, prompt_w_exit, strlen(prompt_w_exit));

    return;
}

int main() {
    // no current foreground process in running
    fg_running = 0;

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
