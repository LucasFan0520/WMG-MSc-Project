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
    j->name = malloc(strlen(name) + 1);
    strcpy(j->name, name);
    j->payload = malloc(strlen(payload) + 1);
    strcpy(j->payload, payload);
    j->next = head;
    head = j;
}

void clone_job(const char *old, const char *new) {
    Job *j = find_job(old);
    if (!j || find_job(new)) return;
    Job *nj = malloc(sizeof(Job));
    nj->name = malloc(strlen(new) + 1);
    strcpy(nj->name, new);
    nj->payload = malloc(strlen(j->payload) + 1);
    strcpy(nj->payload, j->payload);
    nj->next = head;
    head = nj;
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
        Job *tmp = j->next;
        free(j->name);
        free(j->payload);
        free(j);
        j = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], payload[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) {
                const char *p = rest + strlen(name) + 1;
                while (*p == ' ') p++;
                if (*p) enqueue_job(name, p);
                else enqueue_job(name, "");
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            if (sscanf(rest, "%255s %255s", name, payload) == 2) clone_job(name, payload);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(rest, "%255s", name) == 1) cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            if (sscanf(rest, "%255s", name) == 1) run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free_all();
    return 0;
}