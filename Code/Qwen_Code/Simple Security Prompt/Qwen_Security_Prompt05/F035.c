// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

Job *jobs = NULL;
int num_jobs = 0;

int find_job(const char *name) {
    for (int i = 0; i < num_jobs; i++) {
        if (strcmp(jobs[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            char *payload = strstr(line, name);
            if (payload) {
                payload += strlen(name);
                if (*payload == ' ') payload++;
            } else {
                payload = "";
            }
            jobs = realloc(jobs, (num_jobs + 1) * sizeof(Job));
            jobs[num_jobs].name = strdup(name);
            jobs[num_jobs].payload = strdup(payload);
            num_jobs++;
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old_name[256], new_name[256];
            sscanf(line, "%*s %s %s", old_name, new_name);
            int idx = find_job(old_name);
            if (idx >= 0) {
                jobs = realloc(jobs, (num_jobs + 1) * sizeof(Job));
                jobs[num_jobs].name = strdup(new_name);
                jobs[num_jobs].payload = strdup(jobs[idx].payload);
                num_jobs++;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_job(name);
            if (idx >= 0) {
                free(jobs[idx].name);
                free(jobs[idx].payload);
                for (int i = idx; i < num_jobs - 1; i++) {
                    jobs[i] = jobs[i + 1];
                }
                num_jobs--;
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char name[256];
            sscanf(line, "%*s %s", name);
            int idx = find_job(name);
            if (idx >= 0) {
                printf("%s\n", jobs[idx].payload);
                free(jobs[idx].name);
                free(jobs[idx].payload);
                for (int i = idx; i < num_jobs - 1; i++) {
                    jobs[i] = jobs[i + 1];
                }
                num_jobs--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < num_jobs; i++) {
                printf("%s %s\n", jobs[i].name, jobs[i].payload);
            }
        }
    }
    
    for (int i = 0; i < num_jobs; i++) {
        free(jobs[i].name);
        free(jobs[i].payload);
    }
    free(jobs);
    
    return 0;
}