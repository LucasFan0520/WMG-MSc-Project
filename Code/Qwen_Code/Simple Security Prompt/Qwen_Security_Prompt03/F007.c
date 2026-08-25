// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int time; char *label; char *note; } Act;
Act *a = NULL;
int n = 0;

int cmp(const void *x, const void *y) {
    Act *ax = (Act*)x, *ay = (Act*)y;
    if (ax->time != ay->time) return ax->time - ay->time;
    return strcmp(ax->label, ay->label);
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], label[256];
        int t;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %d %s", &t, label);
            char *note = strchr(line, ' '); note++; note = strchr(note, ' '); note++; note = strchr(note, ' ');
            a = realloc(a, (n + 1) * sizeof(Act));
            a[n].time = t; a[n].label = strdup(label); a[n].note = strdup(note ? note : ""); n++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %d %s", &t, label);
            for (int i = 0; i < n; ) {
                if (a[i].time == t && strcmp(a[i].label, label) == 0) {
                    free(a[i].label); free(a[i].note);
                    for (int j = i; j < n - 1; j++) a[j] = a[j + 1];
                    n--;
                } else i++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", label);
            int earliest = -1;
            for (int i = 0; i < n; i++) {
                if (strcmp(a[i].label, label) == 0) {
                    if (earliest == -1 || a[i].time < a[earliest].time) earliest = i;
                }
            }
            if (earliest != -1) printf("%d %s %s\n", a[earliest].time, a[earliest].label, a[earliest].note);
        } else if (strcmp(cmd, "SORT") == 0) {
            qsort(a, n, sizeof(Act), cmp);
            for (int i = 0; i < n; i++) printf("%d %s %s\n", a[i].time, a[i].label, a[i].note);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < n; i++) { free(a[i].label); free(a[i].note); }
            n = 0;
        }
    }
    for (int i = 0; i < n; i++) { free(a[i].label); free(a[i].note); }
    free(a);
    return 0;
}