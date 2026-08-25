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
    if (!j) return;
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (!queue) {
        queue = j;
    } else {
        Job *cur = queue;
        while (cur->next) cur = cur->next;
        cur->next = j;
    }
}

void clone_job(const char *old, const char *new) {
    Job *j = find_job(old);
    if (!j || find_job(new)) return;
    Job *copy = malloc(sizeof(Job));
    if (!copy) return;
    copy->name = strdup(new);
    copy->payload = strdup(j->payload);
    copy->next = NULL;
    Job *cur = queue;
    while (cur->next) cur = cur->next;
    cur->next = copy;
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
    Job *j = queue;
    while (j) {
        printf("%s: ", j->name);
        char *s = j->payload;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        j = j->next;
    }
}

void free_all(void) {
    Job *j = queue;
    while (j) {
        Job *next = j->next;
        free(j->name);
        free(j->payload);
        free(j);
        j = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *payload = space + 1;
            while (*payload == ' ') payload++;
            enqueue(name, payload);
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *old = p;
            char *space = strchr(old, ' ');
            if (!space) continue;
            *space = '\0';
            char *new = space + 1;
            while (*new == ' ') new++;
            clone_job(old, new);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free_all();
    return 0;
}