// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int count = 0;

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        int idx;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            sscanf(line, "%*s %d", &idx);
            if (idx < 0 || idx > count) continue;
            char *p = line;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            lines = realloc(lines, sizeof(char*) * (count + 1));
            for (int i = count; i > idx; i--) lines[i] = lines[i - 1];
            lines[idx] = strdup(p);
            count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = line + 7;
            if (*text == ' ') text++;
            lines = realloc(lines, sizeof(char*) * (count + 1));
            lines[count++] = strdup(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %d", &idx);
            if (idx < 0 || idx >= count) continue;
            free(lines[idx]);
            for (int i = idx; i < count - 1; i++) lines[i] = lines[i + 1];
            count--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            sscanf(line, "%*s %d", &idx);
            if (idx < 0 || idx >= count) continue;
            char *p = line;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            free(lines[idx]);
            lines[idx] = strdup(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                for (int j = 0; lines[i][j]; j++)
                    putchar(lines[i][j] == ' ' ? '_' : lines[i][j]);
                putchar('\n');
            }
        }
    }
    for (int i = 0; i < count; i++) free(lines[i]);
    free(lines);
    return 0;
}