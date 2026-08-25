/* F035.c */
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

Job *find_job(char *name) {
    for (Job *j = head; j; j = j->next)
        if (strcmp(j->name, name) == 0) return j;
    return NULL;
}

void enqueue(char *name, char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (!tail) head = tail = j;
    else { tail->next = j; tail = j; }
}

void clone_job(char *old, char *new) {
    Job *o = find_job(old);
    if (!o || find_job(new)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(new);
    j->payload = strdup(o->payload);
    j->next = NULL;
    if (!tail) head = tail = j;
    else { tail->next = j; tail = j; }
}

void cancel(char *name) {
    Job *prev = NULL;
    for (Job *j = head; j; j = j->next) {
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
    }
}

void run_job(char *name) {
    Job *j = find_job(name);
    if (!j) return;
    printf("%s %s\n", j->name, j->payload);
    // remove it
    cancel(name);
}

void list_all(void) {
    for (Job *j = head; j; j = j->next)
        printf("%s %s\n", j->name, j->payload);
}

void free_all(void) {
    Job *j = head;
    while (j) {
        Job *next = j->next;
        free(j->name);
        free(j->payload);
        free(j);
        j = next;
    }
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "ENQUEUE") == 0 && n == 3) {
            enqueue(arg1, arg2);
        } else if (strcmp(cmd, "CLONE") == 0 && n == 3) {
            clone_job(arg1, arg2);
        } else if (strcmp(cmd, "CANCEL") == 0 && n == 2) {
            cancel(arg1);
        } else if (strcmp(cmd, "RUN") == 0 && n == 2) {
            run_job(arg1);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_all();
        }
    }
    free(line);
    free_all();
    return 0;
}