// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *txt; } Line;
Line *lines = 0; int n = 0, cap = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], rest[1700]; int idx;
        rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "INSERT") || !strcmp(cmd, "PATCH")) sscanf(line, "%*s %d %[^\n]", &idx, rest);
        else if (!strcmp(cmd, "DELETE")) sscanf(line, "%*s %d", &idx);
        else if (!strcmp(cmd, "APPEND")) { char *p = strchr(line, ' '); while(*p==' ') p++; strcpy(rest, p); }

        if (!strcmp(cmd, "INSERT")) {
            if (idx < 0 || idx > n) continue;
            if (n == cap) { cap = cap ? cap*2 : 4; lines = realloc(lines, cap*sizeof(Line)); }
            for (int i=n; i>idx; i--) lines[i] = lines[i-1];
            lines[idx].txt = strdup(rest); n++;
        } else if (!strcmp(cmd, "APPEND")) {
            if (n == cap) { cap = cap ? cap*2 : 4; lines = realloc(lines, cap*sizeof(Line)); }
            lines[n++].txt = strdup(rest);
        } else if (!strcmp(cmd, "DELETE")) {
            if (idx < 0 || idx >= n) continue;
            free(lines[idx].txt);
            for (int i=idx; i<n-1; i++) lines[i] = lines[i+1]; n--;
        } else if (!strcmp(cmd, "PATCH")) {
            if (idx < 0 || idx >= n) continue;
            free(lines[idx].txt); lines[idx].txt = strdup(rest);
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i=0; i<n; i++) {
                for (char *c = lines[i].txt; *c; c++) putchar(*c == ' ' ? '_' : *c);
                putchar('\n');
            }
        }
    }
    return 0;
}