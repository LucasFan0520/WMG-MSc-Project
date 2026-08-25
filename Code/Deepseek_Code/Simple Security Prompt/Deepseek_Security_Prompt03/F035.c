/*
 * F035.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct job {
    char *name;
    char *payload;
    struct job *next;
} Job;

Job *queue = NULL;

Job *find_job(const char *name) {
    Job *j = queue;
    while (j) {
        if (strcmp(j->name, name) == 0) return j;
        j = j->next;
    }
    return NULL;
}

void enqueue_job(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = queue;
    queue = j;
}

void clone_job(const char *old, const char *new) {
    Job *src = find_job(old);
    if (!src || find_job(new)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(new);
    j->payload = strdup(src->payload);
    j->next = queue;
    queue = j;
}

void cancel_job(const char *name) {
    Job *prev = NULL, *j = queue;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else queue = j->next;
            free(j->name);
            free(j->payload);
            free(j);
            return;
        }
        prev = j;
        j = j->next;
    }
}

void run_job(const char *name) {
    Job *j = find_job(name);
    if (!j) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s\n", j->name, j->payload);
    cancel_job(name);
}

void list_jobs(void) {
    Job *j = queue;
    while (j) {
        printf("%s\n", j->name);
        j = j->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[1024];
        if (sscanf(line, "%31s %255s %1023s", cmd, arg1, arg2) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            enqueue_job(arg1, rest);
        } else if (strcmp(cmd, "CLONE") == 0) {
            clone_job(arg1, arg2);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel_job(arg1);
        } else if (strcmp(cmd, "RUN") == 0) {
            run_job(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    return 0;
}