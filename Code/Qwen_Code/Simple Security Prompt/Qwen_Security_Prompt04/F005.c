// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *lines[10000];
int lc = 0;

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            sscanf(rest, "%d", &idx);
            char *text = rest;
            while (*text && *text != ' ') text++;
            while (*text == ' ') text++;
            if (idx < 0 || idx > lc) continue;
            for (int i = lc; i > idx; i--) lines[i] = lines[i-1];
            lines[idx] = strdup(text);
            lc++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            lines[lc++] = strdup(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx;
            sscanf(rest, "%d", &idx);
            if (idx < 0 || idx >= lc) continue;
            free(lines[idx]);
            for (int i = idx; i < lc - 1; i++) lines[i] = lines[i+1];
            lc--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx;
            sscanf(rest, "%d", &idx);
            char *text = rest;
            while (*text && *text != ' ') text++;
            while (*text == ' ') text++;
            if (idx < 0 || idx >= lc) continue;
            free(lines[idx]);
            lines[idx] = strdup(text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < lc; i++) {
                for (char *p = lines[i]; *p; p++) {
                    if (*p == ' ') putchar('_');
                    else putchar(*p);
                }
                putchar('\n');
            }
        }
    }
    for (int i = 0; i < lc; i++) free(lines[i]);
    return 0;
}