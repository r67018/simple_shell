#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>

#define OPTION_MAX 255
#define CMD_MAX 1 + OPTION_MAX + 1 // command + options + null

void child(char **);

void replace_char(char *s, size_t len, char c1, char c2)
{
    size_t i;
    for (i = 0; i < len; i++) {
        if (s[i] == c1) {
            s[i] = c2;
        }
    }
}

pid_t spawn_child(char **cmd)
{
    pid_t pid = fork();
    if (pid == 0) {
        child(cmd);
        _exit(0);
    } else {
        return pid;
    }
}

int main(void)
{
    char *line = NULL;
    size_t len = 0;

    // main loop
    while (1) {
        int i;
        ssize_t line_len;
        char *cmd[CMD_MAX];
        int cmd_cnt = 0;
        bool is_background = false;

        for (i = 0; i < CMD_MAX; i++) {
            cmd[i] = NULL;
        }

        // get one line
        printf("%% ");
        line_len = getline(&line, &len, stdin);
        if (line_len == -1) {
            fprintf(stderr, "Failed to read line.\n");
            continue;
        }

        replace_char(line, line_len, ' ', '\0');
        replace_char(line, line_len, '\n', '\0');
        // decrement for lf replacement
        line_len--;

        // split line with null and store in cmd
        cmd[0] = line;
        cmd_cnt++;
        for (i = 0; i < line_len; i++) {
            if (line[i] == '\0' && line[i + 1] != '\0') {
                cmd[cmd_cnt] = &line[i + 1];
                cmd_cnt++;
            }
        }
        // case of background execution
        if (strcmp(cmd[cmd_cnt - 1], "&") == 0) {
            is_background = true;
            cmd[cmd_cnt - 1] = NULL;
            cmd_cnt--;
        }

        // check if command is exit
        if (strcmp(cmd[0], "exit") == 0) {
            break;
        }

        // check if only newline have been entered
        if (strcmp(cmd[0], "") == 0) {
            continue;
        }

        // spawn child process
        pid_t ch_pid = spawn_child(cmd);
        if (ch_pid == -1) {
            fprintf(stderr, "Failed to create process.\n");
            continue;
        }
        else if (!is_background) {
            int wstatus;
            if (waitpid(ch_pid, &wstatus, 0) == -1) {
                fprintf(stderr, "Failed to wait process.\n");
            }
        }
    }

    free(line);

    return 0;
}

