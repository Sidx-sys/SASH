# SASH, A fully functional shell

## Compliation, Execution

**The shell executable is already provided**, but if you want to rebuild it:

Inside the **submitted** folder, there is a *makefile* to create the `shell` executable. To create the executable type:
                    `make`
This creates the shell executable which can be run through `./shell`

If you want to clean the object files, do :
                    `make clean`
This removes the obj folder and the executable

---


### 1. Input/Output Redirection
The *input and output redirection* was handled in my `Exec` function in the `utils.c` file. It basically **checks for any form of of redirection in the command** (if any exist) and then **accordingly changes the `STDIN` and `STDOUT` to the respective `fd`** of the input/output file. After the execution of the command is over, the `STDIN` and `STDOUT`  are returned to 0 and 1 respectively.

### 2. Command Pipelines
Pipelining is implemented in the `utils.c` file, in the function `piping(char *)`, using the `pipe()` function. After cleaning the input command (`Normalized Input`) the *commmand is sent to the `piping()`* where in the the command is **separated by pipes and the input/output is piped along the commands**. After that *each command is sent to the `Exec` function* and there it executes the command along with any redirection.

### 3. I/O Redirection with Command Pipelines
The Redirection is done inside the `Exec` function and the `piping()` function sends the command to be executed to the `Exec` function. Since **since redirection binds stronger than pipes**, my code already supported the two things in one command.

### 4. User Defined Commands
- setenv => Implemented in the `functions.c` file in the `Setenv` function using the `setenv` syscall
- unsetenv => Implemented in the `functions.c` file in the `Unsetenv` function using the `unsetenv` syscall
- jobs => Implemented in the `functions.c` file in the `Jobs` function
- kjob => Implemented in the `functions.c` file in the `Kjob` function
- fg => Implemented in the `process.c` file in the `Fg` function
- bg => Implemented in the `process.c` file in the `Bg` function
- overkill => Implemented in the `process.c` file in the `Overkill` function
- quit => Implemented in the `utils.c` file and while taking input for `ctrl + D`
  
### 5. Signal Handling
- The two signals were handled in the `main.c` file in the `SigTstpHandler` and the `SigIntHandler` using the `Sigaction` structure.
---

## Bonus Features

### 1.Last Working Directory
Added support for `cd -` in the `ChangeDirectory` function in `functions.c` **using a global character array which stores the just previously visited directory path**.

### 2. Exit Codes
Added Support for Exit code reflection in the command prompt **through the flag `prev_stat` which keeps track of the exit status of the previously executed command** in the shell.
