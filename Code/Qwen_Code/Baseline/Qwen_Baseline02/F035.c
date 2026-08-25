// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *payload; } Job;
Job jobs[10000]; int njobs = 0;

int find_job(char *name) {
    for (int i = 0; i < njobs; i++) if (!strcmp(jobs[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], payload[10000], old[100], new_name[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ENQUEUE")) {
            scanf("%s", name);
            fgets(payload, sizeof(payload), stdin);
            payload[strcspn(payload, "\n")] = 0;
            if (payload[0] == ' ') memmove(payload, payload + 1, strlen(payload));
            jobs[njobs].name = strdup(name);
            jobs[njobs++].payload = strdup(payload);
        } else if (!strcmp(cmd, "CLONE")) {
            scanf("%s %s", old, new_name);
            int idx = find_job(old);
            if (idx != -1) {
                jobs[njobs].name = strdup(new_name);
                jobs[njobs++].payload = strdup(jobs[idx].payload);
            }
        } else if (!strcmp(cmd, "CANCEL")) {
            scanf("%s", name);
            int idx = find_job(name);
            if (idx != -1) {
                free(jobs[idx].name); free(jobs[idx].payload);
                for (int i = idx; i < njobs - 1; i++) jobs[i] = jobs[i + 1];
                njobs--;
            }
        } else if (!strcmp(cmd, "RUN")) {
            scanf("%s", name);
            int idx = find_job(name);
            if (idx != -1) {
                printf("%s\n", jobs[idx].payload);
                free(jobs[idx].name); free(jobs[idx].payload);
                for (int i = idx; i < njobs - 1; i++) jobs[i] = jobs[i + 1];
                njobs--;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < njobs; i++) {
                printf("%s %s\n", jobs[i].name, jobs[i].payload);
            }
        }
    }
    for (int i = 0; i < njobs; i++) { free(jobs[i].name); free(jobs[i].payload); }
    return 0;
}