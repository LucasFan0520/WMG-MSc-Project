// F035.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

Job *queue = NULL;
int count = 0;
int capacity = 0;

int find_job(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(queue[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void enqueue_job(const char *name, const char *payload) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        queue = realloc(queue, capacity * sizeof(Job));
    }
    queue[count].name = strdup(name);
    queue[count].payload = strdup(payload);
    count++;
}

void clone_job(const char *oldName, const char *newName) {
    int idx = find_job(oldName);
    if (idx != -1) {
        enqueue_job(newName, queue[idx].payload);
    }
}

void cancel_job(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(queue[i].name, name) == 0) {
            free(queue[i].name);
            free(queue[i].payload);
            for (int j = i; j < count - 1; j++) {
                queue[j] = queue[j + 1];
            }
            count--;
            i--;
        }
    }
}

void run_job(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(queue[i].name, name) == 0) {
            printf("%s\n", queue[i].payload);
            free(queue[i].name);
            free(queue[i].payload);
            for (int j = i; j < count - 1; j++) {
                queue[j] = queue[j + 1];
            }
            count--;
            return;
        }
    }
}

void list_jobs(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", queue[i].name, queue[i].payload);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *name = strtok(NULL, " ");
            char *payload = strtok(NULL, "");
            if (name && payload) enqueue_job(name, payload);
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *oldName = strtok(NULL, " ");
            char *newName = strtok(NULL, "");
            if (oldName && newName) clone_job(oldName, newName);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = strtok(NULL, "");
            if (name) cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            char *name = strtok(NULL, "");
            if (name) run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].payload);
    }
    free(queue);
    return 0;
}
