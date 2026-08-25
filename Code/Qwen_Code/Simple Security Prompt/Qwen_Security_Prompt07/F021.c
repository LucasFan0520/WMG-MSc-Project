// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *ing, *note; } Ing;
typedef struct { char *name; Ing *ings; int n, cap; } Rec;
Rec *recs = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(recs[i].name, name)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256], rest[2048];
        a[0] = b[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ING")) sscanf(line, "%*s %s %s %[^\n]", a, b, rest);
        else if (!strcmp(cmd, "REMOVEING")) sscanf(line, "%*s %s %s", a, b);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "RECIPE")) {
            if (find(a) < 0) {
                if (n == cap) { cap = cap ? cap * 2 : 4; recs = realloc(recs, cap * sizeof(Rec)); }
                recs[n].name = strdup(a); recs[n].ings = 0; recs[n].n = 0; recs[n].cap = 0; n++;
            }
        } else if (!strcmp(cmd, "ING")) {
            int i = find(a);
            if (i >= 0) {
                if (recs[i].n == recs[i].cap) {
                    recs[i].cap = recs[i].cap ? recs[i].cap * 2 : 4;
                    recs[i].ings = realloc(recs[i].ings, recs[i].cap * sizeof(Ing));
                }
                recs[i].ings[recs[i].n].ing = strdup(b);
                recs[i].ings[recs[i].n].note = strdup(rest);
                recs[i].n++;
            }
        } else if (!strcmp(cmd, "REMOVEING")) {
            int i = find(a);
            if (i >= 0) {
                for (int j = 0; j < recs[i].n; j++) {
                    if (!strcmp(recs[i].ings[j].ing, b)) {
                        free(recs[i].ings[j].ing); free(recs[i].ings[j].note);
                        for (int k = j; k < recs[i].n - 1; k++) recs[i].ings[k] = recs[i].ings[k + 1];
                        recs[i].n--; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETERECIPE")) {
            int i = find(a);
            if (i >= 0) {
                for (int j = 0; j < recs[i].n; j++) { free(recs[i].ings[j].ing); free(recs[i].ings[j].note); }
                free(recs[i].ings); free(recs[i].name);
                for (int j = i; j < n - 1; j++) recs[j] = recs[j + 1]; n--;
            }
        } else if (!strcmp(cmd, "SHOW")) {
            int i = find(a);
            if (i >= 0) {
                printf("%s\n", recs[i].name);
                for (int j = 0; j < recs[i].n; j++) printf("%s %s\n", recs[i].ings[j].ing, recs[i].ings[j].note);
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < n; i++) puts(recs[i].name);
        }
    }
    return 0;
}