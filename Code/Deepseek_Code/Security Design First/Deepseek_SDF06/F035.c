// F035.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
    char *name;
    char *payload;
} Job;

static Job *jobs = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_job(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(jobs[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void enqueue(const char *name, const char *payload) {
    if (find_job(name) != -1) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Job *new = realloc(jobs, capacity * sizeof(Job));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        jobs = new;
    }
    jobs[count].name = strdup(name);
    jobs[count].payload = strdup(payload);
    if (!jobs[count].name || !jobs[count].payload) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void clone_job(const char *old, const char *new) {
    ssize_t idx = find_job(old);
    if (idx == -1) return;
    if (find_job(new) != -1) return;
    enqueue(new, jobs[idx].payload);
}

static void cancel_job(const char *name) {
    ssize_t idx = find_job(name);
    if (idx == -1) return;
    free(jobs[idx].name);
    free(jobs[idx].payload);
    for (size_t i = idx; i < count - 1; i++)
        jobs[i] = jobs[i + 1];
    count--;
}

static void run_job(const char *name) {
    ssize_t idx = find_job(name);
    if (idx == -1) return;
    printf("%s\n", jobs[idx].payload);
    // remove
    free(jobs[idx].name);
    free(jobs[idx].payload);
    for (size_t i = idx; i < count - 1; i++)
        jobs[i] = jobs[i + 1];
    count--;
}

static void list_jobs(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", jobs[i].name, jobs[i].payload);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(jobs[i].name);
        free(jobs[i].payload);
    }
    free(jobs);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], arg[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ENQUEUE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, arg) == 2)
                enqueue(name, arg);
        } else if (strcmp(cmd, "CLONE") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, arg) == 2)
                clone_job(name, arg);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }

    free(line);
    free_all();
    return 0;
}