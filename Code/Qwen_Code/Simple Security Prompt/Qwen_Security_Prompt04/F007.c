// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int time;
    char *label;
    char *note;
} Act;

Act acts[10000];
int ac = 0;

int cmp(const void *a, const void *b) {
    Act *x = (Act *)a;
    Act *y = (Act *)b;
    if (x->time != y->time) return x->time - y->time;
    return strcmp(x->label, y->label);
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            int t;
            char label[1000];
            sscanf(rest, "%d %s", &t, label);
            char *p = rest;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            acts[ac].time = t;
            acts[ac].label = strdup(label);
            acts[ac].note = strdup(p);
            ac++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            int t;
            char label[1000];
            sscanf(rest, "%d %s", &t, label);
            for (int i = 0; i < ac; i++) {
                if (acts[i].time == t && strcmp(acts[i].label, label) == 0) {
                    free(acts[i].label);
                    free(acts[i].note);
                    for (int j = i; j < ac - 1; j++) acts[j] = acts[j+1];
                    ac--;
                    i--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char label[1000];
            sscanf(rest, "%s", label);
            int min_t = 1000000000;
            int idx = -1;
            for (int i = 0; i < ac; i++) {
                if (strcmp(acts[i].label, label) == 0 && acts[i].time < min_t) {
                    min_t = acts[i].time;
                    idx = i;
                }
            }
            if (idx != -1) printf("%d %s %s\n", acts[idx].time, acts[idx].label, acts[idx].note);
        } else if (strcmp(cmd, "SORT") == 0) {
            qsort(acts, ac, sizeof(Act), cmp);
            for (int i = 0; i < ac; i++) printf("%d %s %s\n", acts[i].time, acts[i].label, acts[i].note);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < ac; i++) {
                free(acts[i].label);
                free(acts[i].note);
            }
            ac = 0;
        }
    }
    for (int i = 0; i < ac; i++) {
        free(acts[i].label);
        free(acts[i].note);
    }
    return 0;
}