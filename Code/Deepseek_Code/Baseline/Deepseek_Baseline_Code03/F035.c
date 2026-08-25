// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct job {
    char *name;
    char *payload;
    struct job *next;
};

struct job *jobs = NULL;

struct job *find_job(char *name) {
    struct job *j = jobs;
    while (j) {
        if (strcmp(j->name, name) == 0) return j;
        j = j->next;
    }
    return NULL;
}

void enqueue_job(char *name, char *payload) {
    if (find_job(name)) return;
    struct job *j = malloc(sizeof(struct job));
    j->name = malloc(strlen(name) + 1);
    strcpy(j->name, name);
    j->payload = malloc(strlen(payload) + 1);
    strcpy(j->payload, payload);
    j->next = jobs;
    jobs = j;
}

void clone_job(char *old, char *new) {
    struct job *oldj = find_job(old);
    if (!oldj || find_job(new)) return;
    struct job *j = malloc(sizeof(struct job));
    j->name = malloc(strlen(new) + 1);
    strcpy(j->name, new);
    j->payload = malloc(strlen(oldj->payload) + 1);
    strcpy(j->payload, oldj->payload);
    j->next = jobs;
    jobs = j;
}

void cancel_job(char *name) {
    struct job *j = jobs, *prev = NULL;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else jobs = j->next;
            free(j->name);
            free(j->payload);
            free(j);
            return;
        }
        prev = j;
        j = j->next;
    }
}

void run_job(char *name) {
    struct job *j = find_job(name);
    if (j) {
        printf("%s\n", j->payload);
        cancel_job(name);
    }
}

void list_jobs() {
    struct job *j = jobs;
    while (j) {
        printf("%s %s\n", j->name, j->payload);
        j = j->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, arg1, arg2);
        if (strcmp(cmd, "ENQUEUE") == 0 && n >= 2) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            enqueue_job(arg1, rest);
        } else if (strcmp(cmd, "CLONE") == 0 && n >= 3) {
            clone_job(arg1, arg2);
        } else if (strcmp(cmd, "CANCEL") == 0 && n >= 2) {
            cancel_job(arg1);
        } else if (strcmp(cmd, "RUN") == 0 && n >= 2) {
            run_job(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    return 0;
}