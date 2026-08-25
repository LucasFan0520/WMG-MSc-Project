// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int time; char *label; char *note; } Act;
Act *acts = NULL; int ac = 0;

int cmp_act(const void *a, const void *b) {
    Act *x = (Act*)a, *y = (Act*)b;
    if (x->time != y->time) return x->time - y->time;
    return strcmp(x->label, y->label);
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c3[256], c4[700]; int t;
        c3[0] = c4[0] = 0;
        sscanf(line, "%19s %d %255s %700[^\n]", c1, &t, c3, c4);
        
        if (strcmp(c1, "ADD") == 0) {
            acts = realloc(acts, (ac+1)*sizeof(Act));
            acts[ac].time = t; acts[ac].label = strdup(c3); acts[ac].note = strdup(c4); ac++;
        } else if (strcmp(c1, "DELETE") == 0) {
            for (int i = 0; i < ac; ) {
                if (acts[i].time == t && strcmp(acts[i].label, c3) == 0) {
                    free(acts[i].label); free(acts[i].note);
                    for (int j = i; j < ac-1; j++) acts[j] = acts[j+1];
                    ac--;
                } else i++;
            }
        } else if (strcmp(c1, "FIND") == 0) {
            int min_t = 2147483647, idx = -1;
            for (int i = 0; i < ac; i++) {
                if (strcmp(acts[i].label, c3) == 0 && acts[i].time < min_t) {
                    min_t = acts[i].time; idx = i;
                }
            }
            if (idx >= 0) printf("%d %s %s\n", acts[idx].time, acts[idx].label, acts[idx].note);
        } else if (strcmp(c1, "SORT") == 0) {
            qsort(acts, ac, sizeof(Act), cmp_act);
            for (int i = 0; i < ac; i++) printf("%d %s %s\n", acts[i].time, acts[i].label, acts[i].note);
        } else if (strcmp(c1, "CLEAR") == 0) {
            for (int i = 0; i < ac; i++) { free(acts[i].label); free(acts[i].note); }
            ac = 0;
        }
    }
    for (int i = 0; i < ac; i++) { free(acts[i].label); free(acts[i].note); }
    free(acts);
    return 0;
}