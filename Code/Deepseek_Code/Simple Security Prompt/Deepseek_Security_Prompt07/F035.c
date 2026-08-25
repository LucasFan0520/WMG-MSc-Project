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
    j->next = queue;
    queue = j;
}

void clone_job(const char *oldName, const char *newName) {
    Job *old = find_job(oldName);
    if (!old || find_job(newName)) return;
    enqueue(newName, old->payload);
}

void cancel_job(const char *name) {
    Job *prev = NULL;
    Job *j = queue;
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
    // print and remove
    for (char *s = j->payload; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
    cancel_job(name);
}

void list_jobs() {
    Job *j = queue;
    int first = 1;
    while (j) {
        if (!first) putchar(' ');
        printf("%s", j->name);
        first = 0;
        j = j->next;
    }
    putchar('\n');
}

void free_all() {
    Job *j = queue;
    while (j) {
        Job *next = j->next;
        free(j->name);
        free(j->payload);
        free(j);
        j = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256], payload[4096];
            if (sscanf(rest, "%255s %[^\n]", name, payload) == 2) {
                enqueue(name, payload);
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old[256], new[256];
            if (sscanf(rest, "%255s %255s", old, new) == 2) {
                clone_job(old, new);
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel_job(rest);
        } else if (strcmp(cmd, "RUN") == 0) {
            run_job(rest);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free_all();
    return 0;
}