// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int pri; char *title, *note; } Rem;
Rem *rems = 0; int n = 0, cap = 0;

int find(char *title) {
    for (int i = 0; i < n; i++) if (!strcmp(rems[i].title, title)) return i;
    return -1;
}

int cmp(const void *a, const void *b) {
    return ((Rem *)b)->pri - ((Rem *)a)->pri;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], rest[2048]; int pri;
        a[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD")) sscanf(line, "%*s %d %s %[^\n]", &pri, a, rest);
        else if (!strcmp(cmd, "BUMP")) sscanf(line, "%*s %s %d", a, &pri);
        else if (!strcmp(cmd, "TOP")) sscanf(line, "%*s %d", &pri);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "ADD")) {
            if (n == cap) { cap = cap ? cap * 2 : 4; rems = realloc(rems, cap * sizeof(Rem)); }
            rems[n].pri = pri; rems[n].title = strdup(a); rems[n].note = strdup(rest); n++;
        } else if (!strcmp(cmd, "BUMP")) {
            int i = find(a);
            if (i >= 0) rems[i].pri += pri;
        } else if (!strcmp(cmd, "REMOVE")) {
            int i = find(a);
            if (i >= 0) {
                free(rems[i].title); free(rems[i].note);
                for (int j = i; j < n - 1; j++) rems[j] = rems[j + 1]; n--;
            }
        } else if (!strcmp(cmd, "TOP")) {
            Rem *sorted = malloc(n * sizeof(Rem));
            memcpy(sorted, rems, n * sizeof(Rem));
            qsort(sorted, n, sizeof(Rem), cmp);
            int lim = pri < n ? pri : n;
            for (int i = 0; i < lim; i++) printf("%d %s %s\n", sorted[i].pri, sorted[i].title, sorted[i].note);
            free(sorted);
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < n; i++) printf("%d %s %s\n", rems[i].pri, rems[i].title, rems[i].note);
        }
    }
    return 0;
}