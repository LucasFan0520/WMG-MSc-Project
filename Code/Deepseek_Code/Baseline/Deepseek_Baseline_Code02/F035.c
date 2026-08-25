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

void enqueue_job(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = malloc(strlen(name) + 1);
    strcpy(j->name, name);
    j->payload = malloc(strlen(payload) + 1);
    strcpy(j->payload, payload);
    j->next = NULL;
    if (tail) tail->next = j;
    else head = j;
    tail = j;
}

void clone_job(const char *old, const char *new) {
    Job *j = find_job(old);
    if (!j || find_job(new)) return;
    enqueue_job(new, j->payload);
}

void cancel_job(const char *name) {
    Job *prev = NULL;
    Job *j = head;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else head = j->next;
            if (tail == j) tail = prev;
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
    printf("%s ", j->name);
    for (char *c = j->payload; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
    cancel_job(name);
}

void list_jobs(void) {
    Job *j = head;
    while (j) {
        printf("%s ", j->name);
        for (char *c = j->payload; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            enqueue_job(name, p);
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *old = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            clone_job(old, p);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            cancel_job(p);
        } else if (strcmp(cmd, "RUN") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            run_job(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free_all();
    return 0;
}