// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct job {
    char *name;
    char *payload;
    struct job *next;
} Job;

Job *head = NULL;
Job *tail = NULL;

Job *find_job(const char *name) {
    Job *j = head;
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
    j->next = NULL;
    if (tail) tail->next = j;
    else head = j;
    tail = j;
}

void clone_job(const char *old, const char *new) {
    Job *o = find_job(old);
    if (!o || find_job(new)) return;
    enqueue_job(new, o->payload);
}

void cancel_job(const char *name) {
    Job *j = head, *prev = NULL;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else head = j->next;
            if (j == tail) tail = prev;
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
    if (!j) return;
    printf("%s\n", j->payload);
    cancel_job(name);
}

void list_jobs(void) {
    Job *j = head;
    while (j) {
        printf("%s %s\n", j->name, j->payload);
        j = j->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], payload[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", name, payload) == 2)
                enqueue_job(name, payload);
        } else if (strcmp(cmd, "CLONE") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, payload) == 2)
                clone_job(name, payload);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    return 0;
}