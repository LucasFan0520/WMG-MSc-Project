// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char ***cells; int rows, cols; } Tab;
Tab *tabs = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(tabs[i].name, name)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256]; int r, c;
        a[0] = b[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "TABLE")) sscanf(line, "%*s %s %d %d", a, &r, &c);
        else if (!strcmp(cmd, "SET")) sscanf(line, "%*s %s %d %d %[^\n]", a, &r, &c, b);
        else if (!strcmp(cmd, "GET")) sscanf(line, "%*s %s %d %d", a, &r, &c);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "TABLE")) {
            if (find(a) < 0) {
                if (n == cap) { cap = cap ? cap * 2 : 4; tabs = realloc(tabs, cap * sizeof(Tab)); }
                tabs[n].name = strdup(a); tabs[n].rows = r; tabs[n].cols = c;
                tabs[n].cells = malloc(r * sizeof(char **));
                for (int i = 0; i < r; i++) {
                    tabs[n].cells[i] = malloc(c * sizeof(char *));
                    for (int j = 0; j < c; j++) tabs[n].cells[i][j] = 0;
                }
                n++;
            }
        } else if (!strcmp(cmd, "SET")) {
            int idx = find(a);
            if (idx >= 0 && r >= 0 && r < tabs[idx].rows && c >= 0 && c < tabs[idx].cols) {
                free(tabs[idx].cells[r][c]);
                tabs[idx].cells[r][c] = strdup(b);
            }
        } else if (!strcmp(cmd, "GET")) {
            int idx = find(a);
            if (idx >= 0 && r >= 0 && r < tabs[idx].rows && c >= 0 && c < tabs[idx].cols) {
                if (tabs[idx].cells[r][c]) puts(tabs[idx].cells[r][c]);
                else puts("EMPTY");
            }
        } else if (!strcmp(cmd, "DELETE")) {
            int idx = find(a);
            if (idx >= 0) {
                for (int i = 0; i < tabs[idx].rows; i++) {
                    for (int j = 0; j < tabs[idx].cols; j++) free(tabs[idx].cells[i][j]);
                    free(tabs[idx].cells[i]);
                }
                free(tabs[idx].cells); free(tabs[idx].name);
                for (int i = idx; i < n - 1; i++) tabs[i] = tabs[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "TABLES")) {
            for (int i = 0; i < n; i++) puts(tabs[i].name);
        }
    }
    return 0;
}