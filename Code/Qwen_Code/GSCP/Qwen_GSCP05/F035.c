// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

Job *jobs = NULL;
int jc = 0;

int find_job(const char *name) {
    for (int i = 0; i < jc; i++) {
        if (strcmp(jobs[i].name, name) == 0) return i;
    }
    return -1;
}

int main(void) {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[256];
        char rest[4096];
        a1[0] = '\0';
        a2[0] = '\0';
        rest[0] = '\0';
        int n = sscanf(line, "%31s %255s %255s %4095[^\n]", cmd, a1, a2, rest);
        if (n < 1) continue;

        if (strcmp(cmd, "ENQUEUE") == 0 && n >= 3) {
            jobs = realloc(jobs, (jc + 1) * sizeof(Job));
            if (!jobs) return 1;
            jobs[jc].name = strdup(a1);
            jobs[jc].payload = strdup(a2);
            jc++;
        } else if (strcmp(cmd, "CLONE") == 0 && n >= 3) {
            int oi = find_job(a1);
            if (oi >= 0) {
                jobs = realloc(jobs, (jc + 1) * sizeof(Job));
                if (!jobs) return 1;
                jobs[jc].name = strdup(a2);
                jobs[jc].payload = strdup(jobs[oi].payload);
                jc++;
            }
        } else if (strcmp(cmd, "CANCEL") == 0 && n >= 2) {
            int ji = find_job(a1);
            if (ji >= 0) {
                free(jobs[ji].name);
                free(jobs[ji].payload);
                for (int k = ji; k < jc - 1; k++) {
                    jobs[k] = jobs[k + 1];
                }
                jc--;
            }
        } else if (strcmp(cmd, "RUN") == 0 && n >= 2) {
            int ji = find_job(a1);
            if (ji >= 0) {
                printf("%s %s\n", jobs[ji].name, jobs[ji].payload);
                free(jobs[ji].name);
                free(jobs[ji].payload);
                for (int k = ji; k < jc - 1; k++) {
                    jobs[k] = jobs[k + 1];
                }
                jc--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < jc; i++) {
                printf("%s %s\n", jobs[i].name, jobs[i].payload);
            }
        }
    }
    for (int i = 0; i < jc; i++) {
        free(jobs[i].name);
        free(jobs[i].payload);
    }
    free(jobs);
    return 0;
}