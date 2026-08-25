// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

Job *jobs = NULL;

Job *find_job(const char *name) {
    for (Job *j = jobs; j; j = j->next) {
        if (strcmp(j->name, name) == 0) return j;
    }
    return NULL;
}

void enqueue(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (!jobs) jobs = j;
    else {
        Job *curr = jobs;
        while (curr->next) curr = curr->next;
        curr->next = j;
    }
}

void clone_job(const char *old_name, const char *new_name) {
    Job *old_j = find_job(old_name);
    if (!old_j || find_job(new_name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(new_name);
    j->payload = strdup(old_j->payload);
    j->next = NULL;
    Job *curr = jobs;
    while (curr->next) curr = curr->next;
    curr->next = j;
}

void cancel_job(const char *name) {
    Job **jp = &jobs;
    while (*jp) {
        if (strcmp((*jp)->name, name) == 0) {
            Job *tmp = *jp;
            *jp = tmp->next;
            free(tmp->name);
            free(tmp->payload);
            free(tmp);
            return;
        }
        jp = &(*jp)->next;
    }
}

void run_job(const char *name) {
    Job **jp = &jobs;
    while (*jp) {
        if (strcmp((*jp)->name, name) == 0) {
            Job *tmp = *jp;
            *jp = tmp->next;
            printf("%s\n", tmp->payload);
            free(tmp->name);
            free(tmp->payload);
            free(tmp);
            return;
        }
        jp = &(*jp)->next;
    }
}

void list_jobs() {
    for (Job *j = jobs; j; j = j->next) {
        printf("%s %s\n", j->name, j->payload);
    }
}

void cleanup() {
    while (jobs) {
        Job *tmp = jobs->next;
        free(jobs->name);
        free(jobs->payload);
        free(jobs);
        jobs = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) enqueue(a1, t + 1); }
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) clone_job(a1, a2);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) cancel_job(a1);
        } else if (strcmp(cmd, "RUN") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) run_job(a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    cleanup();
    return 0;
}