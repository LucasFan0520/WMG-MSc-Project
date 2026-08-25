// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *payload; } Job;
Job *jobs = NULL;
int nj = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256], b[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ENQUEUE") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            jobs = realloc(jobs, (nj + 1) * sizeof(Job));
            jobs[nj].name = strdup(a);
            jobs[nj].payload = strdup(t);
            nj++;
        } else if (strcmp(cmd, "CLONE") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            for (int i = 0; i < nj; i++) {
                if (strcmp(jobs[i].name, a) == 0) {
                    jobs = realloc(jobs, (nj + 1) * sizeof(Job));
                    jobs[nj].name = strdup(b);
                    jobs[nj].payload = strdup(jobs[i].payload);
                    nj++;
                    break;
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nj; i++) {
                if (strcmp(jobs[i].name, a) == 0) {
                    free(jobs[i].name); free(jobs[i].payload);
                    for (int j = i; j < nj - 1; j++) jobs[j] = jobs[j + 1];
                    nj--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nj; i++) {
                if (strcmp(jobs[i].name, a) == 0) {
                    printf("%s %s\n", jobs[i].name, jobs[i].payload);
                    free(jobs[i].name); free(jobs[i].payload);
                    for (int j = i; j < nj - 1; j++) jobs[j] = jobs[j + 1];
                    nj--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < nj; i++) {
                printf("%s %s\n", jobs[i].name, jobs[i].payload);
            }
        }
    }
    for (int i = 0; i < nj; i++) { free(jobs[i].name); free(jobs[i].payload); }
    free(jobs);
    return 0;
}