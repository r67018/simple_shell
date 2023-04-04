#include <stdio.h>
#include <unistd.h>

#define _GNU_SOURCE

void child(char **cmd)
{
    int status = execvp(cmd[0], cmd);
    if (status == -1) {
        fprintf(stderr, "command not found\n");
    }
}

