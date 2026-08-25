// F005.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char **lines = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "INSERT") == 0 && args) {
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int index = atoi(args);
                char *text = space2 + 1;
                while (*text == ' ') text++;
                if (index >= 0 && index <= count) {
                    lines = realloc(lines, (count + 1) * sizeof(char *));
                    if (!lines) return 1;
                    for (int j = count; j > index; j--) {
                        lines[j] = lines[j - 1];
                    }
                    lines[index] = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0 && args) {
            lines = realloc(lines, (count + 1) * sizeof(char *));
            if (!lines) return 1;
            lines[count] = strdup(args);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            int index = atoi(args);
            if (index >= 0 && index < count) {
                free(lines[index]);
                for (int j = index; j < count - 1; j++) {
                    lines[j] = lines[j + 1];
                }
                count--;
                lines = realloc(lines, count * sizeof(char *));
                if (count > 0 && !lines) return 1;
            }
        } else if (strcmp(cmd, "PATCH") == 0 && args) {
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int index = atoi(args);
                char *text = space2 + 1;
                while (*text == ' ') text++;
                if (index >= 0 && index < count) {
                    free(lines[index]);
                    lines[index] = strdup(text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                for (int j = 0; lines[i][j] != '\0'; j++) {
                    if (lines[i][j] == ' ') {
                        putchar('_');
                    } else {
                        putchar(lines[i][j]);
                    }
                }
                putchar('\n');
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
