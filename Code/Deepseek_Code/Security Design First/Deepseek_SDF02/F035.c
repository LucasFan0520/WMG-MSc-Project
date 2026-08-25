/* F035.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct job {
    char *name;
    char *payload;
    struct job *next;
} job_t;

job_t *head = NULL;
job_t *tail = NULL;

job_t *find_job(const char *name) {
    job_t *j = head;
    while (j) {
        if (strcmp(j->name, name) == 0) return j;
        j = j->next;
    }
    return NULL;
}

void enqueue(const char *name, const char *payload) {
    if (find_job(name)) return;
    job_t *j = malloc(sizeof(job_t));
    if (!j) return;
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (!j->name || !j->payload) {
        free(j->name); free(j->payload); free(j);
        return;
    }
    if (tail) tail->next = j;
    else head = j;
    tail = j;
}

void clone_job(const char *old, const char *new) {
    job_t *j = find_job(old);
    if (!j) return;
    if (find_job(new)) return;
    enqueue(new, j->payload);
}

void cancel_job(const char *name) {
    job_t *prev = NULL, *j = head;
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
    job_t *j = find_job(name);
    if (!j) return;
    printf("%s %s\n", j->name, j->payload);
    cancel_job(name);
}

void list_jobs(void) {
    job_t *j = head;
    while (j) {
        printf("%s %s\n", j->name, j->payload);
        j = j->next;
    }
}

void free_all(void) {
    job_t *j = head;
    while (j) {
        job_t *nxt = j->next;
        free(j->name);
        free(j->payload);
        free(j);
        j = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256];
        if (sscanf(line, "%9s %255s %255s", cmd, a, b) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "ENQUEUE") == 0) {
                enqueue(a, rest);
            } else if (strcmp(cmd, "CLONE") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    clone_job(a, b);
            } else if (strcmp(cmd, "CANCEL") == 0) {
                cancel_job(a);
            } else if (strcmp(cmd, "RUN") == 0) {
                run_job(a);
            } else if (strcmp(cmd, "LIST") == 0) {
                list_jobs();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}