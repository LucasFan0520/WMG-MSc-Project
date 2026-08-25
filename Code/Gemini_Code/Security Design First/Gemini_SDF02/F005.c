/* F005.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char **lines = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;
        char *arg = p;

        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = arg;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            int idx = atoi(idx_str);
            if (idx >= 0 && (size_t)idx <= count) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    char **temp = realloc(lines, new_cap * sizeof(char *));
                    if (!temp) break;
                    lines = temp;
                    capacity = new_cap;
                }
                for (size_t i = count; i > (size_t)idx; i--) {
                    lines[i] = lines[i - 1];
                }
                lines[idx] = strdup(text);
                if (lines[idx]) {
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                char **temp = realloc(lines, new_cap * sizeof(char *));
                if (!temp) break;
                lines = temp;
                capacity = new_cap;
            }
            lines[count] = strdup(arg);
            if (lines[count]) {
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = atoi(arg);
            if (idx >= 0 && (size_t)idx < count) {
                free(lines[idx]);
                for (size_t i = (size_t)idx; i < count - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_str = arg;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *text = p;
            int idx = atoi(idx_str);
            if (idx >= 0 && (size_t)idx < count) {
                char *temp = strdup(text);
                if (temp) {
                    free(lines[idx]);
                    lines[idx] = temp;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
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
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
