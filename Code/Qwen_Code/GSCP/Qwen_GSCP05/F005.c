// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL; int lc = 0;

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c3[700]; int idx;
        c3[0] = 0;
        sscanf(line, "%19s %d %700[^\n]", c1, &idx, c3);
        if (strcmp(c1, "APPEND") == 0) sscanf(line, "%19s %700[^\n]", c1, c3);
        
        if (strcmp(c1, "INSERT") == 0) {
            if (idx < 0 || idx > lc) continue;
            lines = realloc(lines, (lc+1)*sizeof(char*));
            for (int i = lc; i > idx; i--) lines[i] = lines[i-1];
            lines[idx] = strdup(c3); lc++;
        } else if (strcmp(c1, "APPEND") == 0) {
            lines = realloc(lines, (lc+1)*sizeof(char*));
            lines[lc++] = strdup(c3);
        } else if (strcmp(c1, "DELETE") == 0) {
            if (idx < 0 || idx >= lc) continue;
            free(lines[idx]);
            for (int i = idx; i < lc-1; i++) lines[i] = lines[i+1];
            lc--;
        } else if (strcmp(c1, "PATCH") == 0) {
            if (idx < 0 || idx >= lc) continue;
            free(lines[idx]); lines[idx] = strdup(c3);
        } else if (strcmp(c1, "PRINT") == 0) {
            for (int i = 0; i < lc; i++) {
                for (char *p = lines[i]; *p; p++) putchar(*p==' '?'_':*p);
                putchar('\n');
            }
        }
    }
    for (int i = 0; i < lc; i++) free(lines[i]);
    free(lines);
    return 0;
}