// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *payload; } Job;
Job *jobs = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(jobs[i].name, name)) return i;
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
        if (!strcmp(cmd, "ENQUEUE")) sscanf(line, "%*s %s %[^\n]", a, rest);
        else if (!strcmp(cmd, "CLONE")) sscanf(line, "%*s %s %s", a, b);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "ENQUEUE")) {
            if (n == cap) { cap = cap ? cap * 2 : 4; jobs = realloc(jobs, cap * sizeof(Job)); }
            jobs[n].name = strdup(a); jobs[n].payload = strdup(rest); n++;
        } else if (!strcmp(cmd, "CLONE")) {
            int idx = find(a);
            if (idx >= 0) {
                if (n == cap) { cap = cap ? cap * 2 : 4; jobs = realloc(jobs, cap * sizeof(Job)); }
                jobs[n].name = strdup(b); jobs[n].payload = strdup(jobs[idx].payload); n++;
            }
        } else if (!strcmp(cmd, "CANCEL")) {
            int idx = find(a);
            if (idx >= 0) {
                free(jobs[idx].name); free(jobs[idx].payload);
                for (int i = idx; i < n - 1; i++) jobs[i] = jobs[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "RUN")) {
            int idx = find(a);
            if (idx >= 0) {
                puts(jobs[idx].payload);
                free(jobs[idx].name); free(jobs[idx].payload);
                for (int i = idx; i < n - 1; i++) jobs[i] = jobs[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < n; i++) printf("%s %s\n", jobs[i].name, jobs[i].payload);
        }
    }
    return 0;
}