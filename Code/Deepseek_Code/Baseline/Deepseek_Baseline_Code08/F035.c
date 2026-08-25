// F035.c
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
    Job *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void enqueue(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *new = malloc(sizeof(Job));
    new->name = strdup(name);
    new->payload = strdup(payload);
    new->next = NULL;
    if (!head) head = new;
    else {
        Job *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void clone_job(const char *old, const char *new_name) {
    if (find_job(new_name)) return;
    Job *old_job = find_job(old);
    if (!old_job) return;
    Job *new = malloc(sizeof(Job));
    new->name = strdup(new_name);
    new->payload = strdup(old_job->payload);
    new->next = NULL;
    if (!head) head = new;
    else {
        Job *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void cancel_job(const char *name) {
    Job *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
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
    printf("%s\n", j->payload);
    cancel_job(name);
}

void list_jobs() {
    Job *cur = head;
    while (cur) {
        printf("%s %s\n", cur->name, cur->payload);
        cur = cur->next;
    }
}

void free_all() {
    Job *cur = head;
    while (cur) {
        Job *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->payload);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *payload = strtok(NULL, "");
            if (!payload) payload = "";
            enqueue(name, payload);
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *old = strtok(NULL, " ");
            if (!old) continue;
            char *new = strtok(NULL, " ");
            if (new) clone_job(old, new);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = strtok(NULL, " ");
            if (name) cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            char *name = strtok(NULL, " ");
            if (name) run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free_all();
    return 0;
}