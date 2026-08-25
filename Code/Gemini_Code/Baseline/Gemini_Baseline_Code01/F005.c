// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char **lines = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "INSERT") == 0) {
            int idx = 0;
            int idx_bytes = 0;
            if (sscanf(args, "%d%n", &idx, &idx_bytes) == 1) {
                if (idx >= 0 && idx <= count) {
                    char *text = args + idx_bytes;
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
            lines[count] = strdup(args);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = 0;
            if (sscanf(args, "%d", &idx) == 1) {
                if (idx >= 0 && idx < count) {
                    free(lines[idx]);
                    for (int i = idx; i < count - 1; i++) {
                        lines[i] = lines[i + 1];
                    }
                    count--;
                    lines = realloc(lines, count * sizeof(char *));
                }
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx = 0;
            int idx_bytes = 0;
            if (sscanf(args, "%d%n", &idx, &idx_bytes) == 1) {
                if (idx >= 0 && idx < count) {
                    char *text = args + idx_bytes;
                    while (*text == ' ') text++;
                    free(lines[idx]);
                    lines[idx] = strdup(text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                for (int j = 0; lines[i][j] != '\0'; j++) {
                    if (lines[i][j] == ' ') printf("_");
                    else printf("%c", lines[i][j]);
                }
                printf("\n");
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
    return 0;
}
