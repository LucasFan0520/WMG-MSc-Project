/* F035.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

Job *head = NULL;

Job* find_job(const char *name) {
    Job *j = head;
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
    if (!head) head = j;
    else {
        Job *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = j;
    }
}

void clone_job(const char *old, const char *new) {
    Job *j = find_job(old);
    if (!j || find_job(new)) return;
    enqueue(new, j->payload);
}

void cancel_job(const char *name) {
    Job *j = head, *prev = NULL;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else head = j->next;
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

void list_jobs() {
    Job *j = head;
    while (j) {
        printf("%s %s\n", j->name, j->payload);
        j = j->next;
    }
}

void free_all() {
    while (head) {
        Job *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->payload);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, a, b);
        if (strcmp(cmd, "ENQUEUE") == 0 && n == 3) {
            enqueue(a, b);
        } else if (strcmp(cmd, "CLONE") == 0 && n >= 3) {
            clone_job(a, b);
        } else if (strcmp(cmd, "CANCEL") == 0 && n == 2) {
            cancel_job(a);
        } else if (strcmp(cmd, "RUN") == 0 && n == 2) {
            run_job(a);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_jobs();
        }
    }
    free_all();
    return 0;
}