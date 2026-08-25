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
Job *tail = NULL;

Job *find_job(const char *name) {
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
    j->name = malloc(strlen(name)+1);
    strcpy(j->name, name);
    j->payload = malloc(strlen(payload)+1);
    strcpy(j->payload, payload);
    j->next = NULL;
    if (tail) tail->next = j;
    else head = j;
    tail = j;
}

void clone_job(const char *old, const char *new) {
    Job *j = find_job(old);
    if (!j || find_job(new)) return;
    enqueue(new, j->payload);
}

void cancel_job(const char *name) {
    Job *prev = NULL;
    Job *j = head;
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

void free_all(void) {
    Job *j = head;
    while (j) {
        Job *next = j->next;
        free(j->name);
        free(j->payload);
        free(j);
        j = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *payload = strtok(NULL, "");
            if (!payload) payload = "";
            enqueue(name, payload);
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *old = strtok(rest, " ");
            char *new = strtok(NULL, " ");
            if (old && new) clone_job(old, new);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = strtok(rest, " ");
            if (name) cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            char *name = strtok(rest, " ");
            if (name) run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free_all();
    return 0;
}