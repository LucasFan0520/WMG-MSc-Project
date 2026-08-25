// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job { char *name; char *payload; struct Job *next; } Job;
Job *jobs = NULL;

Job *find_job(const char *name) {
    for (Job *j = jobs; j; j = j->next)
        if (strcmp(j->name, name) == 0) return j;
    return NULL;
}

void enqueue(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    if (!j) return;
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = jobs;
    jobs = j;
}

void clone_job(const char *old, const char *new) {
    Job *src = find_job(old);
    if (!src || find_job(new)) return;
    enqueue(new, src->payload);
}

void cancel_job(const char *name) {
    Job *prev = NULL;
    for (Job *j = jobs; j; prev = j, j = j->next) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next; else jobs = j->next;
            free(j->name); free(j->payload); free(j);
            return;
        }
    }
}

void run_job(const char *name) {
    Job *j = find_job(name);
    if (!j) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", j->payload);
    cancel_job(name);
}

void list_jobs(void) {
    for (Job *j = jobs; j; j = j->next)
        printf("%s\n", j->name);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256], payload[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *p = rest + strlen(name);
                while (*p == ' ') p++;
                strcpy(payload, p);
                enqueue(name, payload);
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old[256], new[256];
            if (sscanf(rest, "%255s %255s", old, new) == 2) clone_job(old, new);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    while (jobs) { Job *j = jobs; jobs = j->next; free(j->name); free(j->payload); free(j); }
    return 0;
}