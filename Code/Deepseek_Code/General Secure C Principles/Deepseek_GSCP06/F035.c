// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

Job *queue = NULL;
Job *tail = NULL;

Job *find_job(const char *name) {
    Job *j = queue;
    while (j) {
        if (strcmp(j->name, name) == 0) return j;
        j = j->next;
    }
    return NULL;
}

void enqueue(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (!queue) queue = tail = j;
    else {
        tail->next = j;
        tail = j;
    }
}

void clone_job(const char *old, const char *new) {
    Job *j = find_job(old);
    if (!j) return;
    if (find_job(new)) return;
    enqueue(new, j->payload);
}

void cancel_job(const char *name) {
    Job *cur = queue, *prev = NULL;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else queue = cur->next;
            if (cur == tail) tail = prev;
            free(cur->name);
            free(cur->payload);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void run_job(const char *name) {
    Job *j = find_job(name);
    if (!j) return;
    printf("%s %s\n", j->name, j->payload);
    cancel_job(name);
}

void list_jobs(void) {
    Job *j = queue;
    while (j) {
        printf("%s %s\n", j->name, j->payload);
        j = j->next;
    }
}

void cleanup_jobs(void) {
    Job *j = queue;
    while (j) {
        Job *tmp = j;
        j = j->next;
        free(tmp->name);
        free(tmp->payload);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *payload = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            enqueue(arg1, payload);
        } else if (strcmp(cmd, "CLONE") == 0 && n >= 3) {
            clone_job(arg1, arg2);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel_job(arg1);
        } else if (strcmp(cmd, "RUN") == 0) {
            run_job(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    cleanup_jobs();
    return 0;
}