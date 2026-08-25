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
    if (!j) return;
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (!j->name || !j->payload) {
        free(j->name); free(j->payload); free(j);
        return;
    }
    if (!head) head = j;
    else {
        Job *last = head;
        while (last->next) last = last->next;
        last->next = j;
    }
}

void clone_job(const char *old, const char *new) {
    Job *src = find_job(old);
    if (!src || find_job(new)) return;
    enqueue_job(new, src->payload);
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
    printf("%s %s\n", j->name, j->payload);
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], old[256], new[256], payload[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", old, payload) == 2)
                enqueue_job(old, payload);
        } else if (strcmp(cmd, "CLONE") == 0) {
            if (sscanf(line, "%*s %255s %255s", old, new) == 2)
                clone_job(old, new);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", old) == 1)
                cancel_job(old);
        } else if (strcmp(cmd, "RUN") == 0) {
            if (sscanf(line, "%*s %255s", old) == 1)
                run_job(old);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free(line);
    free_all();
    return 0;
}