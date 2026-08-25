// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int time;
    char *label;
    char *note;
} Act;

Act *acts = NULL;
int count = 0;

int cmp(const void *a, const void *b) {
    Act *x = (Act*)a, *y = (Act*)b;
    if (x->time != y->time) return x->time - y->time;
    return strcmp(x->label, y->label);
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        int t;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %d %s", &t, arg1);
            char *p = line;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            acts = realloc(acts, sizeof(Act) * (count + 1));
            acts[count].time = t;
            acts[count].label = strdup(arg1);
            acts[count].note = strdup(p);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %d %s", &t, arg1);
            for (int i = 0; i < count; ) {
                if (acts[i].time == t && strcmp(acts[i].label, arg1) == 0) {
                    free(acts[i].label);
                    free(acts[i].note);
                    for (int j = i; j < count - 1; j++) acts[j] = acts[j + 1];
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", arg1);
            int min_t = 2000000000, idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(acts[i].label, arg1) == 0 && acts[i].time < min_t) {
                    min_t = acts[i].time;
                    idx = i;
                }
            }
            if (idx != -1) printf("%d %s %s\n", acts[idx].time, acts[idx].label, acts[idx].note);
        } else if (strcmp(cmd, "SORT") == 0) {
            qsort(acts, count, sizeof(Act), cmp);
            for (int i = 0; i < count; i++) printf("%d %s %s\n", acts[i].time, acts[i].label, acts[i].note);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(acts[i].label);
                free(acts[i].note);
            }
            count = 0;
        }
    }
    for (int i = 0; i < count; i++) {
        free(acts[i].label);
        free(acts[i].note);
    }
    free(acts);
    return 0;
}