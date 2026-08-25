// F005.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int count = 0;
int capacity = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "INSERT") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    int idx = atoi(arg1);
                    char *text = space2 + 1;
                    if (idx >= 0 && idx <= count) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            char **tmp = realloc(lines, capacity * sizeof(char*));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            lines = tmp;
                        }
                        for (int i = count; i > idx; i--) {
                            lines[i] = lines[i - 1];
                        }
                        lines[idx] = strdup(text);
                        if (lines[idx]) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "APPEND") == 0) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    char **tmp = realloc(lines, capacity * sizeof(char*));
                    if (!tmp) {
                        free(line);
                        return 1;
                    }
                    lines = tmp;
                }
                lines[count] = strdup(arg1);
                if (lines[count]) {
                    count++;
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                int idx = atoi(arg1);
                if (idx >= 0 && idx < count) {
                    free(lines[idx]);
                    for (int i = idx; i < count - 1; i++) {
                        lines[i] = lines[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "PATCH") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    int idx = atoi(arg1);
                    char *text = space2 + 1;
                    if (idx >= 0 && idx < count) {
                        char *nt = strdup(text);
                        if (nt) {
                            free(lines[idx]);
                            lines[idx] = nt;
                        }
                    }
                }
            }
        } else {
            if (strcmp(cmd, "PRINT") == 0) {
                for (int i = 0; i < count; i++) {
                    char *p = lines[i];
                    for (size_t j = 0; p[j] != '\0'; j++) {
                        if (p[j] == ' ') putchar('_');
                        else putchar(p[j]);
                    }
                    putchar('\n');
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
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