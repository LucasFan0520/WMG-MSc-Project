/* F035.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct job {
    char *name;
    char *payload;
    struct job *next;
} Job;

Job *front = NULL, *rear = NULL;

Job *find_job(const char *name) {
    Job *j = front;
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
    if (rear) rear->next = j;
    else front = j;
    rear = j;
}

void clone_job(const char *old, const char *new) {
    Job *o = find_job(old);
    if (!o || find_job(new)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(new);
    j->payload = strdup(o->payload);
    j->next = NULL;
    if (rear) rear->next = j;
    else front = j;
    rear = j;
}

void cancel_job(const char *name) {
    Job *j = front, *prev = NULL;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else front = j->next;
            if (!front) rear = NULL;
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
    Job *j = front, *prev = NULL;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else front = j->next;
            if (!front) rear = NULL;
            printf("%s ", j->name);
            for (char *c = j->payload; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
            free(j->name);
            free(j->payload);
            free(j);
            return;
        }
        prev = j;
        j = j->next;
    }
}

void list_jobs() {
    Job *j = front;
    while (j) {
        printf("%s ", j->name);
        for (char *c = j->payload; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        j = j->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], newname[256], payload[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "LIST") == 0) { list_jobs(); continue; }
        if (strcmp(cmd, "ENQUEUE") == 0) {
            if (sscanf(line, "%*s %255s", name) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            enqueue(name, rest);
        } else if (strcmp(cmd, "CLONE") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, newname) == 2)
                clone_job(name, newname);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) run_job(name);
        }
    }
    return 0;
}