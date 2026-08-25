// F030.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_pattern;
    char *new_pattern;
} RenameRule;

int main(void) {
    RenameRule *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *old_p = p;
            while (*p && *p != ' ') p++;
            char *new_p = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                new_p = p;
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                RenameRule *tmp = realloc(arr, capacity * sizeof(RenameRule));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].old_pattern = strdup(old_p);
            arr[count].new_pattern = strdup(new_p);
            if (!arr[count].old_pattern || !arr[count].new_pattern) exit(1);
            count++;
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            while (*text == ' ') text++;
            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                char *pos = strstr(text, arr[i].old_pattern);
                if (pos) {
                    size_t old_len = strlen(arr[i].old_pattern);
                    size_t prefix_len = pos - text;
                    printf("%.*s%s%s\n", (int)prefix_len, text, arr[i].new_pattern, pos + old_len);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old_p = line + 7;
            while (*old_p == ' ') old_p++;
            for (size_t i = 0; i < count; ) {
                if (strcmp(arr[i].old_pattern, old_p) == 0) {
                    free(arr[i].old_pattern);
                    free(arr[i].new_pattern);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *i_str = p;
            while (*p && *p != ' ') p++;
            char *j_str = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                j_str = p;
            }
            int idx_i = atoi(i_str);
            int idx_j = atoi(j_str);
            if (idx_i >= 0 && (size_t)idx_i < count && idx_j >= 0 && (size_t)idx_j < count && idx_i != idx_j) {
                RenameRule tmp = arr[idx_i];
                arr[idx_i] = arr[idx_j];
                arr[idx_j] = tmp;
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%zu: %s -> %s\n", i, arr[i].old_pattern, arr[i].new_pattern);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].old_pattern);
        free(arr[i].new_pattern);
    }
    free(arr);
    free(line);
    return 0;
}
