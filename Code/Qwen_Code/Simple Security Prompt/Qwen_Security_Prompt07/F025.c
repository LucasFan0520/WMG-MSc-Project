// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; } Stud;
typedef struct { char *name; Stud *studs; int n, cap; } Cls;
Cls *clss = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(clss[i].name, name)) return i;
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
        if (!strcmp(cmd, "STUDENT")) sscanf(line, "%*s %s %s %[^\n]", a, b, rest);
        else if (!strcmp(cmd, "TRANSFER")) sscanf(line, "%*s %s %s %s", a, b, c);
        else if (!strcmp(cmd, "DROP")) sscanf(line, "%*s %s %s", a, b);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "CLASS")) {
            if (find(a) < 0) {
                if (n == cap) { cap = cap ? cap * 2 : 4; clss = realloc(clss, cap * sizeof(Cls)); }
                clss[n].name = strdup(a); clss[n].studs = 0; clss[n].n = 0; clss[n].cap = 0; n++;
            }
        } else if (!strcmp(cmd, "STUDENT")) {
            int i = find(a);
            if (i >= 0) {
                if (clss[i].n == clss[i].cap) {
                    clss[i].cap = clss[i].cap ? clss[i].cap * 2 : 4;
                    clss[i].studs = realloc(clss[i].studs, clss[i].cap * sizeof(Stud));
                }
                clss[i].studs[clss[i].n].name = strdup(b);
                clss[i].studs[clss[i].n].note = strdup(rest);
                clss[i].n++;
            }
        } else if (!strcmp(cmd, "TRANSFER")) {
            int iF = find(a), iT = find(b);
            if (iF >= 0 && iT >= 0) {
                for (int j = 0; j < clss[iF].n; j++) {
                    if (!strcmp(clss[iF].studs[j].name, c)) {
                        if (clss[iT].n == clss[iT].cap) {
                            clss[iT].cap = clss[iT].cap ? clss[iT].cap * 2 : 4;
                            clss[iT].studs = realloc(clss[iT].studs, clss[iT].cap * sizeof(Stud));
                        }
                        clss[iT].studs[clss[iT].n++] = clss[iF].studs[j];
                        for (int k = j; k < clss[iF].n - 1; k++) clss[iF].studs[k] = clss[iF].studs[k + 1];
                        clss[iF].n--; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DROP")) {
            int i = find(a);
            if (i >= 0) {
                for (int j = 0; j < clss[i].n; j++) {
                    if (!strcmp(clss[i].studs[j].name, b)) {
                        free(clss[i].studs[j].name); free(clss[i].studs[j].note);
                        for (int k = j; k < clss[i].n - 1; k++) clss[i].studs[k] = clss[i].studs[k + 1];
                        clss[i].n--; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETECLASS")) {
            int i = find(a);
            if (i >= 0) {
                for (int j = 0; j < clss[i].n; j++) { free(clss[i].studs[j].name); free(clss[i].studs[j].note); }
                free(clss[i].studs); free(clss[i].name);
                for (int j = i; j < n - 1; j++) clss[j] = clss[j + 1]; n--;
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < n; i++) {
                printf("%s\n", clss[i].name);
                for (int j = 0; j < clss[i].n; j++) printf("%s %s\n", clss[i].studs[j].name, clss[i].studs[j].note);
            }
        }
    }
    return 0;
}