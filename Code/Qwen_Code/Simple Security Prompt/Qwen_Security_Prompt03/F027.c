// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int priority; char *title; char *note; int order; } Rem;
Rem *rems = NULL;
int nr = 0;
int global_order = 0;

int cmp_top(const void *x, const void *y) {
    Rem *a = (Rem*)x, *b = (Rem*)y;
    if (a->priority != b->priority) return b->priority - a->priority;
    return a->order - b->order;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        int p;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %d", &p);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            char *note = strchr(t, ' ');
            char title[256];
            strncpy(title, t, note ? note - t : strlen(t));
            title[note ? note - t : strlen(t)] = 0;
            
            rems = realloc(rems, (nr + 1) * sizeof(Rem));
            rems[nr].priority = p;
            rems[nr].title = strdup(title);
            rems[nr].note = strdup(note ? note + 1 : "");
            rems[nr].order = global_order++;
            nr++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            sscanf(line, "%*s %s %d", a, &p);
            for (int i = 0; i < nr; i++) {
                if (strcmp(rems[i].title, a) == 0) {
                    rems[i].priority += p;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nr; i++) {
                if (strcmp(rems[i].title, a) == 0) {
                    free(rems[i].title);
                    free(rems[i].note);
                    for (int j = i; j < nr - 1; j++) rems[j] = rems[j + 1];
                    nr--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            sscanf(line, "%*s %d", &p);
            Rem *sorted = malloc(nr * sizeof(Rem));
            memcpy(sorted, rems, nr * sizeof(Rem));
            qsort(sorted, nr, sizeof(Rem), cmp_top);
            int limit = p < nr ? p : nr;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
            }
            free(sorted);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < nr; i++) {
                printf("%d %s %s\n", rems[i].priority, rems[i].title, rems[i].note);
            }
        }
    }
    for (int i = 0; i < nr; i++) {
        free(rems[i].title);
        free(rems[i].note);
    }
    free(rems);
    return 0;
}