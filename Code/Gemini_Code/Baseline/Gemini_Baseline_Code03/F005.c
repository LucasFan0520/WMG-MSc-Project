// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char **lines = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "INSERT") == 0) {
            int idx = -1;
            int idx_len = 0;
            if (sscanf(ptr, "%d%n", &idx, &idx_len) > 0) {
                if (idx >= 0 && idx <= count) {
                    char *text = ptr + idx_len;
                    while (*text == ' ') text++;
                    lines = realloc(lines, (count + 1) * sizeof(char *));
                    for (int i = count; i > idx; i--) {
                        lines[i] = lines[i - 1];
                    }
                    lines[idx] = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            lines = realloc(lines, (count + 1) * sizeof(char *));
            lines[count] = strdup(ptr);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = -1;
            if (sscanf(ptr, "%d", &idx) > 0) {
                if (idx >= 0 && idx < count) {
                    free(lines[idx]);
                    for (int i = idx; i < count - 1; i++) {
                        lines[i] = lines[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        lines = realloc(lines, count * sizeof(char *));
                    } else {
                        free(lines);
                        lines = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx = -1;
            int idx_len = 0;
            if (sscanf(ptr, "%d%n", &idx, &idx_len) > 0) {
                if (idx >= 0 && idx < count) {
                    char *text = ptr + idx_len;
                    while (*text == ' ') text++;
                    free(lines[idx]);
                    lines[idx] = strdup(text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                char *t = lines[i];
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
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
