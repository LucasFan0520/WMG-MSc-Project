// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char **lines = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (idx_s && text) {
                int idx = atoi(idx_s);
                if (idx >= 0 && idx <= count) {
                    lines = realloc(lines, (count + 1) * sizeof(char *));
                    for (int i = count; i > idx; i--) {
                        lines[i] = lines[i - 1];
                    }
                    lines[idx] = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = strtok(NULL, "");
            if (text) {
                lines = realloc(lines, (count + 1) * sizeof(char *));
                lines[count] = strdup(text);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_s = strtok(NULL, " ");
            if (idx_s) {
                int idx = atoi(idx_s);
                if (idx >= 0 && idx < count) {
                    free(lines[idx]);
                    for (int i = idx; i < count - 1; i++) {
                        lines[i] = lines[i + 1];
                    }
                    count--;
                    if (count == 0) {
                        free(lines);
                        lines = NULL;
                    } else {
                        lines = realloc(lines, count * sizeof(char *));
                    }
                }
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_s = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (idx_s && text) {
                int idx = atoi(idx_s);
                if (idx >= 0 && idx < count) {
                    free(lines[idx]);
                    lines[idx] = strdup(text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                char *t = lines[i];
                for (int j = 0; t[j]; j++) {
                    if (t[j] == ' ') {
                        putchar('_');
                    } else {
                        putchar(t[j]);
                    }
                }
                putchar('\n');
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
