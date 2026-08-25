/* F035.c */
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
    j->next = NULL;
    if (!queue) queue = j;
    else {
        Job *cur = queue;
        while (cur->next) cur = cur->next;
        cur->next = j;
    }
}

void clone_job(const char *old, const char *new) {
    Job *o = find_job(old);
    if (!o) return;
    if (find_job(new)) return;
    enqueue(new, o->payload);
}

void cancel_job(const char *name) {
    Job *j = queue, *prev = NULL;
    while (j && strcmp(j->name, name) != 0) {
        prev = j;
        j = j->next;
    }
    if (!j) return;
    if (prev) prev->next = j->next;
    else queue = j->next;
    free(j->name);
    free(j->payload);
    free(j);
}

void run_job(const char *name) {
    Job *j = find_job(name);
    if (!j) { printf("NOT_FOUND\n"); return; }
    printf("%s ", j->name);
    for (char *c = j->payload; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
    cancel_job(name);
}

void list_jobs(void) {
    Job *j = queue;
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256];
            char *p = line + 8;
            if (sscanf(p, "%255s", name) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            enqueue(name, p);
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old[256], new[256];
            if (sscanf(line + 6, "%255s %255s", old, new) == 2)
                clone_job(old, new);
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
    free(line);
    return 0;
}