// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

Job *head = NULL, *tail = NULL;

Job *find_job(const char *name) {
    Job *j = head;
    while (j) {
        if (strcmp(j->name, name) == 0) return j;
        j = j->next;
    }
    return NULL;
}

void enqueue_job(const char *name, const char *payload) {
    Job *j = malloc(sizeof(Job));
    j->name = malloc(strlen(name) + 1);
    strcpy(j->name, name);
    j->payload = malloc(strlen(payload) + 1);
    strcpy(j->payload, payload);
    j->next = NULL;
    if (tail) {
        tail->next = j;
        tail = j;
    } else {
        head = tail = j;
    }
}

void clone_job(const char *old, const char *new) {
    Job *j = find_job(old);
    if (!j) return;
    if (find_job(new)) return;
    Job *clone = malloc(sizeof(Job));
    clone->name = malloc(strlen(new) + 1);
    strcpy(clone->name, new);
    clone->payload = malloc(strlen(j->payload) + 1);
    strcpy(clone->payload, j->payload);
    clone->next = NULL;
    if (tail) {
        tail->next = clone;
        tail = clone;
    } else {
        head = tail = clone;
    }
}

void cancel_job(const char *name) {
    Job *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->payload);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void run_job(const char *name) {
    Job *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            printf("%s %s\n", curr->name, curr->payload);
            free(curr->name);
            free(curr->payload);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_jobs(void) {
    Job *j = head;
    while (j) {
        printf("%s %s\n", j->name, j->payload);
        j = j->next;
    }
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *name = strtok(NULL, " ");
            char *payload = strtok(NULL, "");
            if (name && payload) enqueue_job(name, payload);
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *old = strtok(NULL, " ");
            char *new = strtok(NULL, " ");
            if (old && new) clone_job(old, new);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = strtok(NULL, " ");
            if (name) cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            char *name = strtok(NULL, " ");
            if (name) run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free(line);
    free_all();
    return 0;
}