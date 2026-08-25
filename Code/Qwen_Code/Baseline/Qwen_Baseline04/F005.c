// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int line_count = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            scanf("%d", &idx);
            char text[1000];
            scanf(" %[^\n]", text);
            if (idx < 0 || idx > line_count) continue;
            lines = realloc(lines, sizeof(char*) * (line_count + 1));
            for (int i = line_count; i > idx; i--) lines[i] = lines[i-1];
            lines[idx] = strdup(text);
            line_count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            char text[1000];
            scanf(" %[^\n]", text);
            lines = realloc(lines, sizeof(char*) * (line_count + 1));
            lines[line_count++] = strdup(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx;
            scanf("%d", &idx);
            if (idx < 0 || idx >= line_count) continue;
            free(lines[idx]);
            for (int i = idx; i < line_count - 1; i++) lines[i] = lines[i+1];
            line_count--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx;
            scanf("%d", &idx);
            char text[1000];
            scanf(" %[^\n]", text);
            if (idx < 0 || idx >= line_count) continue;
            free(lines[idx]);
            lines[idx] = strdup(text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < line_count; i++) {
                for (char *p = lines[i]; *p; p++) {
                    if (*p == ' ') putchar('_');
                    else putchar(*p);
                }
                putchar('\n');
            }
        }
    }
    for (int i = 0; i < line_count; i++) free(lines[i]);
    free(lines);
    return 0;
}