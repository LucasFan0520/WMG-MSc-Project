/* F005.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char **lines = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "INSERT ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (!space) continue;
            *space = '\0';
            int idx = atoi(args);
            char *text = space + 1;
            if (idx >= 0 && idx <= count) {
                char *n_text = strdup(text);
                if (n_text) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        char **tmp = realloc(lines, new_cap * sizeof(char *));
                        if (tmp) {
                            lines = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_text);
                            *space = ' ';
                            continue;
                        }
                    }
                    for (int i = count; i > idx; i--) {
                        lines[i] = lines[i - 1];
                    }
                    lines[idx] = n_text;
                    count++;
                }
            }
            *space = ' ';
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *text = line + 7;
            char *n_text = strdup(text);
            if (n_text) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    char **tmp = realloc(lines, new_cap * sizeof(char *));
                    if (tmp) {
                        lines = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_text);
                        continue;
                    }
                }
                lines[count] = n_text;
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            int idx = atoi(line + 7);
            if (idx >= 0 && idx < count) {
                free(lines[idx]);
                for (int i = idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "PATCH ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (!space) continue;
            *space = '\0';
            int idx = atoi(args);
            char *text = space + 1;
            if (idx >= 0 && idx < count) {
                char *n_text = strdup(text);
                if (n_text) {
                    free(lines[idx]);
                    lines[idx] = n_text;
                }
            }
            *space = ' ';
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                char *p = lines[i];
                while (*p) {
                    if (*p == ' ') putchar('_');
                    else putchar(*p);
                    p++;
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
