// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int time; char *label, *note; } Act;
Act *acts = 0; int n = 0, cap = 0;

int cmp(const void *a, const void *b) {
    Act *x = (Act*)a, *y = (Act*)b;
    if (x->time != y->time) return x->time - y->time;
    return strcmp(x->label, y->label);
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], label[256], rest[1700]; int time;
        rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD")) sscanf(line, "%*s %d %s %[^\n]", &time, label, rest);
        else if (!strcmp(cmd, "DELETE")) sscanf(line, "%*s %d %s", &time, label);
        else sscanf(line, "%*s %s", label);

        if (!strcmp(cmd, "ADD")) {
            if (n == cap) { cap = cap ? cap*2 : 4; acts = realloc(acts, cap*sizeof(Act)); }
            acts[n].time = time; acts[n].label = strdup(label); acts[n].note = strdup(rest); n++;
        } else if (!strcmp(cmd, "DELETE")) {
            for (int i=0; i<n; ) {
                if (!strcmp(acts[i].label, label) && acts[i].time == time) {
                    free(acts[i].label); free(acts[i].note);
                    for (int j=i; j<n-1; j++) acts[j] = acts[j+1]; n--;
                } else i++;
            }
        } else if (!strcmp(cmd, "FIND")) {
            int min_t = 1e9, idx = -1;
            for (int i=0; i<n; i++) {
                if (!strcmp(acts[i].label, label) && acts[i].time < min_t) {
                    min_t = acts[i].time; idx = i;
                }
            }
            if (idx >= 0) printf("%d %s %s\n", acts[idx].time, acts[idx].label, acts[idx].note);
        } else if (!strcmp(cmd, "SORT")) {
            qsort(acts, n, sizeof(Act), cmp);
            for (int i=0; i<n; i++) printf("%d %s %s\n", acts[i].time, acts[i].label, acts[i].note);
        } else if (!strcmp(cmd, "CLEAR")) {
            for (int i=0; i<n; i++) { free(acts[i].label); free(acts[i].note); }
            n = 0;
        }
    }
    return 0;
}