// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id, *name, *dept, *title; } Emp;
Emp *emps = 0; int n = 0, cap = 0;

int find(char *id) {
    for (int i = 0; i < n; i++) if (!strcmp(emps[i].id, id)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256], c[256], d[256];
        a[0] = b[0] = c[0] = d[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD") || !strcmp(cmd, "UPDATE")) sscanf(line, "%*s %s %s %s %s", a, b, c, d);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "ADD")) {
            if (find(a) < 0) {
                if (n == cap) { cap = cap ? cap * 2 : 4; emps = realloc(emps, cap * sizeof(Emp)); }
                emps[n].id = strdup(a); emps[n].name = strdup(b); emps[n].dept = strdup(c); emps[n].title = strdup(d); n++;
            }
        } else if (!strcmp(cmd, "UPDATE")) {
            int idx = find(a);
            if (idx >= 0) {
                free(emps[idx].name); free(emps[idx].dept); free(emps[idx].title);
                emps[idx].name = strdup(b); emps[idx].dept = strdup(c); emps[idx].title = strdup(d);
            }
        } else if (!strcmp(cmd, "DELETE")) {
            int idx = find(a);
            if (idx >= 0) {
                free(emps[idx].id); free(emps[idx].name); free(emps[idx].dept); free(emps[idx].title);
                for (int i = idx; i < n - 1; i++) emps[i] = emps[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            int idx = find(a);
            if (idx >= 0) printf("%s %s %s %s\n", emps[idx].id, emps[idx].name, emps[idx].dept, emps[idx].title);
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < n; i++) printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
        }
    }
    return 0;
}