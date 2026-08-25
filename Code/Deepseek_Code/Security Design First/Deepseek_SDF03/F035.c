// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_PAYLOAD 4096

typedef struct Job {
    char name[MAX_NAME];
    char payload[MAX_PAYLOAD];
    struct Job *next;
} Job;

Job *head = NULL;

Job *find_job(const char *name) {
    Job *j = head;
    while (j) {
        if (strcmp(j->name, name) == 0) return j;
        j = j->next;
    }
    return NULL;
}

void free_all() {
    Job *j = head;
    while (j) {
        Job *tmp = j;
        j = j->next;
        free(tmp);
    }
    head = NULL;
}

void handle_enqueue(char *line) {
    char name[MAX_NAME], payload[MAX_PAYLOAD];
    int n = sscanf(line, "ENQUEUE %63s %4095[^\n]", name, payload);
    if (n < 2) return;
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    if (!j) return;
    strcpy(j->name, name);
    strcpy(j->payload, payload);
    j->next = head;
    head = j;
}

void handle_clone(char *line) {
    char old[MAX_NAME], new[MAX_NAME];
    if (sscanf(line, "CLONE %63s %63s", old, new) != 2) return;
    Job *orig = find_job(old);
    if (!orig) return;
    if (find_job(new)) return;
    Job *j = malloc(sizeof(Job));
    if (!j) return;
    strcpy(j->name, new);
    strcpy(j->payload, orig->payload);
    j->next = head;
    head = j;
}

void handle_cancel(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "CANCEL %63s", name) != 1) return;
    Job *j = head, *prev = NULL;
    while (j) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else head = j->next;
            free(j);
            return;
        }
        prev = j;
        j = j->next;
    }
}

void handle_run(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "RUN %63s", name) != 1) return;
    Job *j = find_job(name);
    if (!j) return;
    printf("%s\n", j->payload);
    // remove
    Job *cur = head, *prev = NULL;
    while (cur) {
        if (cur == j) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
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
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ENQUEUE ", 8) == 0) handle_enqueue(line);
        else if (strncmp(line, "CLONE ", 6) == 0) handle_clone(line);
        else if (strncmp(line, "CANCEL ", 7) == 0) handle_cancel(line);
        else if (strncmp(line, "RUN ", 4) == 0) handle_run(line);
        else if (strcmp(line, "LIST") == 0) handle_list();
    }
    free_all();
    return 0;
}