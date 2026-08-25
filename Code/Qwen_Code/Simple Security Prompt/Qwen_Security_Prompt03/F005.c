// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int n = 0;

void print_under(const char *s) { for(;*s;s++) printf("%c", *s==' '?'_':*s); printf("\n"); }

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20];
        int idx;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "INSERT") == 0) {
            sscanf(line, "%*s %d", &idx);
            if (idx < 0 || idx > n) continue;
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            lines = realloc(lines, (n + 1) * sizeof(char*));
            for (int i = n; i > idx; i--) lines[i] = lines[i - 1];
            lines[idx] = strdup(t ? t : "");
            n++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *t = strchr(line, ' ');
            lines = realloc(lines, (n + 1) * sizeof(char*));
            lines[n++] = strdup(t ? t + 1 : "");
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %d", &idx);
            if (idx < 0 || idx >= n) continue;
            free(lines[idx]);
            for (int i = idx; i < n - 1; i++) lines[i] = lines[i + 1];
            n--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            sscanf(line, "%*s %d", &idx);
            if (idx < 0 || idx >= n) continue;
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            free(lines[idx]);
            lines[idx] = strdup(t ? t : "");
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < n; i++) print_under(lines[i]);
        }
    }
    for (int i = 0; i < n; i++) free(lines[i]);
    free(lines);
    return 0;
}