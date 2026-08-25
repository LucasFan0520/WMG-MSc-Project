// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *note; int done; } Task;
typedef struct { char *name; Task *tasks; int n, cap; } Proj;
Proj *projs = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(projs[i].name, name)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256], c[256], rest[2048];
        a[0] = b[0] = c[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "TASK")) sscanf(line, "%*s %s %s %[^\n]", a, b, rest);
        else if (!strcmp(cmd, "MOVETASK")) sscanf(line, "%*s %s %s %s", a, b, c);
        else if (!strcmp(cmd, "DONETASK")) sscanf(line, "%*s %s %s", a, b);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "PROJECT")) {
            if (find(a) < 0) {
                if (n == cap) { cap = cap ? cap * 2 : 4; projs = realloc(projs, cap * sizeof(Proj)); }
                projs[n].name = strdup(a); projs[n].tasks = 0; projs[n].n = 0; projs[n].cap = 0; n++;
            }
        } else if (!strcmp(cmd, "TASK")) {
            int i = find(a);
            if (i >= 0) {
                if (projs[i].n == projs[i].cap) {
                    projs[i].cap = projs[i].cap ? projs[i].cap * 2 : 4;
                    projs[i].tasks = realloc(projs[i].tasks, projs[i].cap * sizeof(Task));
                }
                projs[i].tasks[projs[i].n].title = strdup(b);
                projs[i].tasks[projs[i].n].note = strdup(rest);
                projs[i].tasks[projs[i].n].done = 0;
                projs[i].n++;
            }
        } else if (!strcmp(cmd, "MOVETASK")) {
            int iF = find(a), iT = find(b);
            if (iF >= 0 && iT >= 0) {
                for (int j = 0; j < projs[iF].n; j++) {
                    if (!strcmp(projs[iF].tasks[j].title, c)) {
                        if (projs[iT].n == projs[iT].cap) {
                            projs[iT].cap = projs[iT].cap ? projs[iT].cap * 2 : 4;
                            projs[iT].tasks = realloc(projs[iT].tasks, projs[iT].cap * sizeof(Task));
                        }
                        projs[iT].tasks[projs[iT].n++] = projs[iF].tasks[j];
                        for (int k = j; k < projs[iF].n - 1; k++) projs[iF].tasks[k] = projs[iF].tasks[k + 1];
                        projs[iF].n--; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DONETASK")) {
            int i = find(a);
            if (i >= 0) {
                for (int j = 0; j < projs[i].n; j++) {
                    if (!strcmp(projs[i].tasks[j].title, b)) {
                        projs[i].tasks[j].done = 1; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETEPROJECT")) {
            int i = find(a);
            if (i >= 0) {
                for (int j = 0; j < projs[i].n; j++) { free(projs[i].tasks[j].title); free(projs[i].tasks[j].note); }
                free(projs[i].tasks); free(projs[i].name);
                for (int j = i; j < n - 1; j++) projs[j] = projs[j + 1]; n--;
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < n; i++) {
                printf("%s\n", projs[i].name);
                for (int j = 0; j < projs[i].n; j++) {
                    printf("%s %s %s\n", projs[i].tasks[j].title, projs[i].tasks[j].note, projs[i].tasks[j].done ? "DONE" : "");
                }
            }
        }
    }
    return 0;
}