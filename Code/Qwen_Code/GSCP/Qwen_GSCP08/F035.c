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

Job *find_job(char *name) {
    for (Job *j = head; j; j = j->next) {
        if (strcmp(j->name, name) == 0) return j;
    }
    return NULL;
}

void enqueue_job(char *name, char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (!head) {
        head = j;
    } else {
        Job *last = head;
        while (last->next) last = last->next;
        last->next = j;
    }
}

void clone_job(char *old_name, char *new_name) {
    Job *old = find_job(old_name);
    if (!old || find_job(new_name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(new_name);
    j->payload = strdup(old->payload);
    j->next = NULL;
    if (!head) {
        head = j;
    } else {
        Job *last = head;
        while (last->next) last = last->next;
        last->next = j;
    }
}

void cancel_job(char *name) {
    Job *prev = NULL;
    Job *curr = head;
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

void run_job(char *name) {
    Job *prev = NULL;
    Job *curr = head;
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

int main() {
    char cmd[20], a[100], b[100], payload[2000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ENQUEUE") == 0) {
            scanf("%99s", a);
            fgets(payload, sizeof(payload), stdin);
            while (payload[0] == ' ') memmove(payload, payload + 1, strlen(payload));
            payload[strcspn(payload, "\n")] = 0;
            enqueue_job(a, payload);
        } else if (strcmp(cmd, "CLONE") == 0) {
            scanf("%99s%99s", a, b);
            clone_job(a, b);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            scanf("%99s", a);
            cancel_job(a);
        } else if (strcmp(cmd, "RUN") == 0) {
            scanf("%99s", a);
            run_job(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    return 0;
}