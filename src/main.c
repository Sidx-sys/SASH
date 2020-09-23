#include "headers.h"

int main() {
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