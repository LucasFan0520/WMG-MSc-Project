// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

Job *jobs = NULL;
int j_count = 0;

int find_name(const char *name) {
    for (int i = 0; i < j_count; i++) {
        if (strcmp(jobs[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char name[1024];
    char new_name[1024];
    char payload[10240];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ENQUEUE") == 0) {
            scanf("%s", name);
            fgets(payload, sizeof(payload), stdin);
            if (payload[0] == ' ') {
                memmove(payload, payload + 1, strlen(payload));
            }
            payload[strcspn(payload, "\n")] = '\0';
            jobs = realloc(jobs, sizeof(Job) * (j_count + 1));
            jobs[j_count].name = strdup(name);
            jobs[j_count].payload = strdup(payload);
            j_count++;
        } else if (strcmp(cmd, "CLONE") == 0) {
            scanf("%s %s", name, new_name);
            int idx = find_name(name);
            if (idx != -1) {
                jobs = realloc(jobs, sizeof(Job) * (j_count + 1));
                jobs[j_count].name = strdup(new_name);
                jobs[j_count].payload = strdup(jobs[idx].payload);
                j_count++;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(jobs[idx].name);
                free(jobs[idx].payload);
                for (int i = idx; i < j_count - 1; i++) {
                    jobs[i] = jobs[i + 1];
                }
                j_count--;
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                printf("%s %s\n", jobs[idx].name, jobs[idx].payload);
                free(jobs[idx].name);
                free(jobs[idx].payload);
                for (int i = idx; i < j_count - 1; i++) {
                    jobs[i] = jobs[i + 1];
                }
                j_count--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < j_count; i++) {
                printf("%s %s\n", jobs[i].name, jobs[i].payload);
            }
        }
    }

    for (int i = 0; i < j_count; i++) {
        free(jobs[i].name);
        free(jobs[i].payload);
    }
    free(jobs);

    return 0;
}