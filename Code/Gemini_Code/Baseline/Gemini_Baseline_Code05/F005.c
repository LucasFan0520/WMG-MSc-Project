// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int count = 0;
int capacity = 0;

void append_line(const char *text) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        lines = realloc(lines, capacity * sizeof(char *));
    }
    lines[count] = strdup(text);
    count++;
}

void insert_line(int idx, const char *text) {
    if (idx < 0 || idx > count) {
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        lines = realloc(lines, capacity * sizeof(char *));
    }
    for (int i = count; i > idx; i--) {
        lines[i] = lines[i - 1];
    }
    lines[idx] = strdup(text);
    count++;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "INSERT ", 7) == 0) {
            char *p1 = line + 7;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                insert_line(atoi(p1), p2 + 1);
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            append_line(line + 7);
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
            char *p1 = line + 6;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int idx = atoi(p1);
                if (idx >= 0 && idx < count) {
                    free(lines[idx]);
                    lines[idx] = strdup(p2 + 1);
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                char *t = lines[i];
                while (*t) {
                    if (*t == ' ') {
                        putchar('_');
                    } else {
                        putchar(*t);
                    }
                    t++;
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
