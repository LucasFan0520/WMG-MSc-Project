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
    Job *curr = jobs;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void enqueue_job(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    if (!j) exit(EXIT_FAILURE);
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (!jobs) {
        jobs = j;
    } else {
        Job *curr = jobs;
        while (curr->next) curr = curr->next;
        curr->next = j;
    }
}

void clone_job(const char *old_name, const char *new_name) {
    Job *old_j = find_job(old_name);
    if (!old_j || find_job(new_name)) return;
    Job *new_j = malloc(sizeof(Job));
    if (!new_j) exit(EXIT_FAILURE);
    new_j->name = strdup(new_name);
    new_j->payload = strdup(old_j->payload);
    new_j->next = NULL;
    if (!jobs) {
        jobs = new_j;
    } else {
        Job *curr = jobs;
        while (curr->next) curr = curr->next;
        curr->next = new_j;
    }
}

void cancel_job(const char *name) {
    Job *curr = jobs;
    Job *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else jobs = curr->next;
            free(curr->name);
            free(curr->payload);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void run_job(const char *name) {
    Job *curr = jobs;
    Job *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            printf("%s\n", curr->payload);
            if (prev) prev->next = curr->next;
            else jobs = curr->next;
            free(curr->name);
            free(curr->payload);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_jobs() {
    Job *curr = jobs;
    while (curr) {
        printf("%s %s\n", curr->name, curr->payload);
        curr = curr->next;
    }
}

void free_all() {
    Job *curr = jobs;
    while (curr) {
        Job *next = curr->next;
        free(curr->name);
        free(curr->payload);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[3800] = {0};
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, arg1, arg2);
        
        if (strcmp(cmd, "ENQUEUE") == 0 && matched >= 2) {
            enqueue_job(arg1, arg2);
        } else if (strcmp(cmd, "CLONE") == 0 && matched >= 2) {
            clone_job(arg1, arg2);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel_job(arg1);
        } else if (strcmp(cmd, "RUN") == 0) {
            run_job(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free_all();
    return 0;
}