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

void enqueue(const char *name, const char *payload) {
    Job *j = malloc(sizeof(Job));
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (!head) head = j;
    else {
        Job *p = head;
        while (p->next) p = p->next;
        p->next = j;
    }
}

void clone_job(const char *old, const char *new_name) {
    for (Job *j = head; j; j = j->next) {
        if (strcmp(j->name, old) == 0) {
            enqueue(new_name, j->payload);
            return;
        }
    }
}

void cancel_job(const char *name) {
    Job *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name); free(p->payload); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void run_job(const char *name) {
    Job *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            printf("%s\n", p->payload);
            free(p->name); free(p->payload); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void list_jobs() {
    for (Job *j = head; j; j = j->next) {
        printf("%s %s\n", j->name, j->payload);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[8000];
        b[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "ENQUEUE") == 0) {
            sscanf(line, "%*s %s %[^\n]", a, b);
            enqueue(a, b);
        } else if (strcmp(cmd, "CLONE") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            clone_job(a, b);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            sscanf(line, "%*s %s", a);
            cancel_job(a);
        } else if (strcmp(cmd, "RUN") == 0) {
            sscanf(line, "%*s %s", a);
            run_job(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    while (head) {
        Job *n = head->next;
        free(head->name); free(head->payload); free(head);
        head = n;
    }
    return 0;
}