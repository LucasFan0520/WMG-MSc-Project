// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

Job *head = NULL;

Job* find_job(const char *name) {
    Job *j = head;
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

void handle_enqueue(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    if (find_job(name)) return;
    char *payload = strtok(NULL, "");
    if (!payload) payload = "";
    Job *new = malloc(sizeof(Job));
    new->name = strdup(name);
    new->payload = strdup(payload);
    new->next = NULL;
    if (!head) head = new;
    else {
        Job *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_clone(char *cmd) {
    char *old = strtok(NULL, " ");
    if (!old) return;
    char *new = strtok(NULL, " ");
    if (!new) return;
    Job *orig = find_job(old);
    if (!orig || find_job(new)) return;
    Job *clone = malloc(sizeof(Job));
    clone->name = strdup(new);
    clone->payload = strdup(orig->payload);
    clone->next = NULL;
    if (!head) head = clone;
    else {
        Job *last = head;
        while (last->next) last = last->next;
        last->next = clone;
    }
}

void handle_cancel(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Job *j = head, *prev = NULL;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else head = j->next;
            free_job(j);
            return;
        }
        prev = j;
        j = j->next;
    }
}

void handle_run(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Job *j = head, *prev = NULL;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else head = j->next;
            printf("%s\n", j->payload);
            free_job(j);
            return;
        }
        prev = j;
        j = j->next;
    }
}

void handle_list() {
    Job *j = head;
    while (j) {
        printf("%s %s\n", j->name, j->payload);
        j = j->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ENQUEUE") == 0) handle_enqueue(cmd);
        else if (strcmp(cmd, "CLONE") == 0) handle_clone(cmd);
        else if (strcmp(cmd, "CANCEL") == 0) handle_cancel(cmd);
        else if (strcmp(cmd, "RUN") == 0) handle_run(cmd);
        else if (strcmp(cmd, "LIST") == 0) handle_list();
    }
    while (head) {
        Job *tmp = head;
        head = head->next;
        free_job(tmp);
    }
    return 0;
}