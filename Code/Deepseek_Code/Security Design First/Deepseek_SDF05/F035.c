// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void enqueue(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    if (!j) return;
    j->name = safe_strdup(name);
    j->payload = safe_strdup(payload);
    j->next = queue;
    queue = j;
}

void clone_job(const char *old, const char *new) {
    Job *old_job = find_job(old);
    if (!old_job) return;
    if (find_job(new)) return;
    Job *j = malloc(sizeof(Job));
    if (!j) return;
    j->name = safe_strdup(new);
    j->payload = safe_strdup(old_job->payload);
    j->next = queue;
    queue = j;
}

void cancel_job(const char *name) {
    Job *prev = NULL, *cur = queue;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else queue = cur->next;
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
    Job *prev = NULL, *cur = queue;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else queue = cur->next;
            printf("%s\n", cur->payload);
            free(cur->name);
            free(cur->payload);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void list_jobs(void) {
    Job *cur = queue;
    while (cur) {
        printf("%s %s\n", cur->name, cur->payload);
        cur = cur->next;
    }
}

void free_all(void) {
    Job *cur = queue;
    while (cur) {
        Job *next = cur->next;
        free(cur->name);
        free(cur->payload);
        free(cur);
        cur = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "ENQUEUE") == 0 && n >= 2) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            enqueue(arg1, p);
        } else if (strcmp(cmd, "CLONE") == 0 && n >= 3) {
            clone_job(arg1, arg2);
        } else if (strcmp(cmd, "CANCEL") == 0 && n >= 2) {
            cancel_job(arg1);
        } else if (strcmp(cmd, "RUN") == 0 && n >= 2) {
            run_job(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free_all();
    return 0;
}