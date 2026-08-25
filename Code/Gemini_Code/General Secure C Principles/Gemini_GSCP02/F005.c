// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char **lines = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "INSERT ", 7) == 0) {
            int idx;
            char text[1024];
            if (sscanf(line + 7, "%d %[^\n]", &idx, text) == 2) {
                if (idx >= 0 && idx <= count) {
                    char **tmp = realloc(lines, (count + 1) * sizeof(char *));
                    if (tmp) {
                        lines = tmp;
                        for (int j = count; j > idx; j--) {
                            lines[j] = lines[j - 1];
                        }
                        lines[idx] = strdup(text);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            while (*text == ' ') {
                text++;
            }
            char **tmp = realloc(lines, (count + 1) * sizeof(char *));
            if (tmp) {
                lines = tmp;
                lines[count] = strdup(text);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            int idx;
            if (sscanf(line + 7, "%d", &idx) == 1) {
                if (idx >= 0 && idx < count) {
                    free(lines[idx]);
                    for (int j = idx; j < count - 1; j++) {
                        lines[j] = lines[j + 1];
                    }
                    count--;
                }
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            int idx;
            char text[1024];
            if (sscanf(line + 6, "%d %[^\n]", &idx, text) == 2) {
                if (idx >= 0 && idx < count) {
                    char *nt = strdup(text);
                    if (nt) {
                        free(lines[idx]);
                        lines[idx] = nt;
                    }
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                for (int k = 0; lines[i][k]; k++) {
                    putchar(lines[i][k] == ' ' ? '_' : lines[i][k]);
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
