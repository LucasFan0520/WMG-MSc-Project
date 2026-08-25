/* F035.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_job(Job *j) {
    free(j->name);
    free(j->payload);
    free(j);
}

void enqueue(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (!queue) {
        queue = j;
    } else {
        Job *last = queue;
        while (last->next) last = last->next;
        last->next = j;
    }
}

void clone_job(const char *old, const char *new) {
    Job *src = find_job(old);
    if (!src || find_job(new)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(new);
    j->payload = strdup(src->payload);
    j->next = NULL;
    if (!queue) {
        queue = j;
    } else {
        Job *last = queue;
        while (last->next) last = last->next;
        last->next = j;
    }
}

void cancel_job(const char *name) {
    Job *prev = NULL, *curr = queue;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else queue = curr->next;
            free_job(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void run_job(const char *name) {
    Job *prev = NULL, *curr = queue;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else queue = curr->next;
            printf("%s\n", curr->payload);
            free_job(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_jobs(void) {
    Job *j = queue;
    while (j) {
        printf("%s\n", j->name);
        j = j->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256], newname[256], payload[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            enqueue(name, p);
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, newname) == 2) clone_job(name, newname);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }

    while (queue) {
        Job *tmp = queue;
        queue = queue->next;
        free_job(tmp);
    }
    return 0;
}