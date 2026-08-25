// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *payload; } Job;
Job *jobs; int n_job;

int find_job(char *name) {
    for(int i=0; i<n_job; i++) if(!strcmp(jobs[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], old[100], nw[100], payload[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "ENQUEUE")) {
            scanf("%s", name);
            fgets(payload, 1000, stdin);
            while(payload[0]==' ') memmove(payload, payload+1, strlen(payload));
            payload[strcspn(payload, "\n")] = 0;
            jobs = realloc(jobs, (n_job+1)*sizeof(Job));
            jobs[n_job].name = strdup(name);
            jobs[n_job].payload = strdup(payload);
            n_job++;
        } else if(!strcmp(cmd, "CLONE")) {
            scanf("%s %s", old, nw);
            int idx = find_job(old);
            if(idx != -1) {
                jobs = realloc(jobs, (n_job+1)*sizeof(Job));
                jobs[n_job].name = strdup(nw);
                jobs[n_job].payload = strdup(jobs[idx].payload);
                n_job++;
            }
        } else if(!strcmp(cmd, "CANCEL")) {
            scanf("%s", name);
            int idx = find_job(name);
            if(idx != -1) {
                free(jobs[idx].name); free(jobs[idx].payload);
                for(int k=idx; k<n_job-1; k++) jobs[k] = jobs[k+1];
                n_job--;
            }
        } else if(!strcmp(cmd, "RUN")) {
            scanf("%s", name);
            int idx = find_job(name);
            if(idx != -1) {
                printf("%s\n", jobs[idx].payload);
                free(jobs[idx].name); free(jobs[idx].payload);
                for(int k=idx; k<n_job-1; k++) jobs[k] = jobs[k+1];
                n_job--;
            }
        } else if(!strcmp(cmd, "LIST")) {
            for(int i=0; i<n_job; i++) printf("%s %s\n", jobs[i].name, jobs[i].payload);
        }
    }
    return 0;
}