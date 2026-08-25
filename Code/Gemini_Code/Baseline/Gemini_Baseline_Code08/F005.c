// F005.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int count = 0;
int capacity = 0;

void append_l(const char *text) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        lines = realloc(lines, capacity * sizeof(char *));
    }
    lines[count] = strdup(text);
    count++;
}

void insert_l(int idx, const char *text) {
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

void delete_l(int idx) {
    if (idx < 0 || idx >= count) {
        return;
    }
    free(lines[idx]);
    for (int i = idx; i < count - 1; i++) {
        lines[i] = lines[i + 1];
    }
    count--;
}

void patch_l(int idx, const char *text) {
    if (idx < 0 || idx >= count) {
        return;
    }
    free(lines[idx]);
    lines[idx] = strdup(text);
}

void print_l(void) {
    for (int i = 0; i < count; i++) {
        char *t = lines[i];
        for (int j = 0; t[j] != '\0'; j++) {
            if (t[j] == ' ') {
                putchar('_');
            } else {
                putchar(t[j]);
            }
        }
        putchar('\n');
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (idx_s && text) {
                int idx = atoi(idx_s);
                insert_l(idx, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = strtok(NULL, "");
            if (text) append_l(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_s = strtok(NULL, " ");
            if (idx_s) {
                int idx = atoi(idx_s);
                delete_l(idx);
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_s = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (idx_s && text) {
                int idx = atoi(idx_s);
                patch_l(idx, text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_l();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
