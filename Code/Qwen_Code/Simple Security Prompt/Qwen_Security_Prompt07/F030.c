// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *old, *new; } Rule;
Rule *rules = 0; int n = 0, cap = 0;

int find(char *old) {
    for (int i = 0; i < n; i++) if (!strcmp(rules[i].old, old)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256]; int i, j;
        a[0] = b[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD")) sscanf(line, "%*s %s %s", a, b);
        else if (!strcmp(cmd, "APPLY")) sscanf(line, "%*s %s", a);
        else if (!strcmp(cmd, "DELETE")) sscanf(line, "%*s %s", a);
        else if (!strcmp(cmd, "SWAP")) sscanf(line, "%*s %d %d", &i, &j);

        if (!strcmp(cmd, "ADD")) {
            if (find(a) < 0) {
                if (n == cap) { cap = cap ? cap * 2 : 4; rules = realloc(rules, cap * sizeof(Rule)); }
                rules[n].old = strdup(a); rules[n].new = strdup(b); n++;
            }
        } else if (!strcmp(cmd, "APPLY")) {
            int idx = -1;
            for (int k = 0; k < n; k++) {
                if (strstr(a, rules[k].old)) { idx = k; break; }
            }
            if (idx >= 0) {
                char *pos = strstr(a, rules[idx].old);
                char res[4096];
                int len = pos - a;
                strncpy(res, a, len); res[len] = 0;
                strcat(res, rules[idx].new);
                strcat(res, pos + strlen(rules[idx].old));
                puts(res);
            } else puts(a);
        } else if (!strcmp(cmd, "DELETE")) {
            int idx = find(a);
            if (idx >= 0) {
                free(rules[idx].old); free(rules[idx].new);
                for (int k = idx; k < n - 1; k++) rules[k] = rules[k + 1]; n--;
            }
        } else if (!strcmp(cmd, "SWAP")) {
            if (i >= 0 && i < n && j >= 0 && j < n) {
                Rule tmp = rules[i]; rules[i] = rules[j]; rules[j] = tmp;
            }
        } else if (!strcmp(cmd, "RULES")) {
            for (int k = 0; k < n; k++) printf("%s %s\n", rules[k].old, rules[k].new);
        }
    }
    return 0;
}