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

void enqueue_job(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    if (!j) return;
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = queue;
    queue = j;
}

void clone_job(const char *old, const char *new) {
    Job *j = find_job(old);
    if (!j || find_job(new)) return;
    enqueue_job(new, j->payload);
}

void cancel_job(const char *name) {
    Job *j = queue, *prev = NULL;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else queue = j->next;
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
    Job *j = queue;
    while (j) {
        printf("%s %s\n", j->name, j->payload);
        j = j->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%15s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "ENQUEUE") == 0 && n >= 3) {
            char *payload = line + strlen(cmd) + strlen(arg1) + 2;
            while (*payload == ' ') payload++;
            enqueue_job(arg1, payload);
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
    while (queue) {
        Job *tmp = queue;
        queue = queue->next;
        free(tmp->name);
        free(tmp->payload);
        free(tmp);
    }
    return 0;
}