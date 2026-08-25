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

void enqueue(const char *name, const char *payload) {
    Job *j = malloc(sizeof(Job));
    if (!j) return;
    j->name = strdup(name);
    j->payload = strdup(payload);
    if (!j->name || !j->payload) { free(j->name); free(j->payload); free(j); return; }
    j->next = NULL;
    if (!head) {
        head = j;
    } else {
        Job *curr = head;
        while (curr->next) curr = curr->next;
        curr->next = j;
    }
}

void clone_job(const char *old_name, const char *new_name) {
    for (Job *j = head; j; j = j->next) {
        if (strcmp(j->name, old_name) == 0) {
            enqueue(new_name, j->payload);
            return;
        }
    }
}

void cancel_job(const char *name) {
    Job *curr = head;
    Job *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
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
    Job *curr = head;
    Job *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            printf("%s\n", curr->payload);
            free(curr->name);
            free(curr->payload);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_jobs() {
    for (Job *j = head; j; j = j->next) {
        printf("%s %s\n", j->name, j->payload);
    }
}

void free_all() {
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
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "ENQUEUE") == 0 && n >= 3) {
            enqueue(arg1, arg2);
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