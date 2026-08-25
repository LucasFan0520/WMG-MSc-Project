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
    if (!j) return;
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (tail) {
        tail->next = j;
        tail = j;
    } else {
        head = tail = j;
    }
}

void clone_job(const char *old, const char *new) {
    Job *o = find_job(old);
    if (!o || find_job(new)) return;
    enqueue_job(new, o->payload);
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
    for (char *p = j->payload; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
    cancel_job(name);
}

void list_jobs(void) {
    Job *j = head;
    while (j) {
        printf("%s ", j->name);
        for (char *p = j->payload; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        j = j->next;
    }
}

void free_all(void) {
    while (head) {
        Job *next = head->next;
        free(head->name);
        free(head->payload);
        free(head);
        head = next;
    }
    tail = NULL;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256], payload[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, payload) == 2) {
                enqueue_job(name, payload);
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old[256], new[256];
            if (sscanf(line, "%*s %255s %255s", old, new) == 2) {
                clone_job(old, new);
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                cancel_job(name);
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                run_job(name);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }

    free(line);
    free_all();
    return 0;
}