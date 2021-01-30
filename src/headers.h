#ifndef HEADERS_H
#define HEADERS_H

// all the std header files used
#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

// all the custom header files used
#include "functions.h"
#include "ls.h"
#include "process.h"
#include "utils.h"

// maros to make the code cleaner
#define MAX_LIMIT 4096
#define MAX_CMD_LIMIT 40000
#define MID_LIMIT 1024
#define S_LIMIT 512
#define NUM_LIMIT 50

// error stream
#define STDERR 2

#endif