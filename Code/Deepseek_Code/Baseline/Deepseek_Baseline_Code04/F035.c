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
    j->next = queue;
    queue = j;
}

void clone_job(const char *old, const char *new) {
    Job *o = find_job(old);
    if (!o || find_job(new)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(new);
    j->payload = strdup(o->payload);
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
    Job *j = queue;
    while (j) {
        printf("%s\n", j->name);
        j = j->next;
    }
}

void free_queue(void) {
    while (queue) {
        Job *tmp = queue;
        queue = queue->next;
        free(tmp->name);
        free(tmp->payload);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256], newname[256];
        int n = sscanf(line, "%s %s %s", cmd, name, newname);
        if (strcmp(cmd, "ENQUEUE") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            if (*p) enqueue(name, p);
        } else if (strcmp(cmd, "CLONE") == 0) {
            if (n < 3) continue;
            clone_job(name, newname);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (n < 2) continue;
            cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            if (n < 2) continue;
            run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free_queue();
    return 0;
}