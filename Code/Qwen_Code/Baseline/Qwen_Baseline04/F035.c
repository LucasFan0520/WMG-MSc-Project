// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

Job *jobs = NULL;
int jcount = 0;

int find_job(const char *name) {
    for (int i = 0; i < jcount; i++) {
        if (strcmp(jobs[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[100], payload[1000];
            scanf("%s", name);
            scanf(" %[^\n]", payload);
            jobs = realloc(jobs, sizeof(Job) * (jcount + 1));
            jobs[jcount].name = strdup(name);
            jobs[jcount].payload = strdup(payload);
            jcount++;
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old[100], new_name[100];
            scanf("%s %s", old, new_name);
            int idx = find_job(old);
            if (idx != -1) {
                jobs = realloc(jobs, sizeof(Job) * (jcount + 1));
                jobs[jcount].name = strdup(new_name);
                jobs[jcount].payload = strdup(jobs[idx].payload);
                jcount++;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_job(name);
            if (idx != -1) {
                free(jobs[idx].name);
                free(jobs[idx].payload);
                for (int i = idx; i < jcount - 1; i++) jobs[i] = jobs[i+1];
                jcount--;
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_job(name);
            if (idx != -1) {
                printf("%s\n", jobs[idx].payload);
                free(jobs[idx].name);
                free(jobs[idx].payload);
                for (int i = idx; i < jcount - 1; i++) jobs[i] = jobs[i+1];
                jcount--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < jcount; i++) {
                printf("%s %s\n", jobs[i].name, jobs[i].payload);
            }
        }
    }
    for (int i = 0; i < jcount; i++) {
        free(jobs[i].name);
        free(jobs[i].payload);
    }
    free(jobs);
    return 0;
}