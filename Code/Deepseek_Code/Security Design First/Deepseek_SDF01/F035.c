// filename: F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

static Job *jobs = NULL;

static void cleanup(void) {
    while (jobs) {
        Job *tmp = jobs;
        jobs = jobs->next;
        free(tmp->name);
        free(tmp->payload);
        free(tmp);
    }
}

static Job *find_job(const char *name) {
    for (Job *j = jobs; j; j = j->next)
        if (strcmp(j->name, name) == 0)
            return j;
    return NULL;
}

static void enqueue(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    if (!j) return;
    j->name = strdup(name);
    j->payload = strdup(payload);
    if (!j->name || !j->payload) {
        free(j->name);
        free(j->payload);
        free(j);
        return;
    }
    j->next = jobs;
    jobs = j;
}

static void clone_job(const char *old, const char *new) {
    Job *orig = find_job(old);
    if (!orig || find_job(new)) return;
    Job *j = malloc(sizeof(Job));
    if (!j) return;
    j->name = strdup(new);
    j->payload = strdup(orig->payload);
    if (!j->name || !j->payload) {
        free(j->name);
        free(j->payload);
        free(j);
        return;
    }
    j->next = jobs;
    jobs = j;
}

static void cancel_job(const char *name) {
    Job *prev = NULL;
    for (Job *j = jobs; j; j = j->next) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else jobs = j->next;
            free(j->name);
            free(j->payload);
            free(j);
            return;
        }
        prev = j;
    }
}

static void run_job(const char *name) {
    Job *prev = NULL;
    for (Job *j = jobs; j; j = j->next) {
        if (strcmp(j->name, name) == 0) {
            if (prev) prev->next = j->next;
            else jobs = j->next;
            printf("%s %s\n", j->name, j->payload);
            free(j->name);
            free(j->payload);
            free(j);
            return;
        }
        prev = j;
    }
}

static void list_jobs(void) {
    for (Job *j = jobs; j; j = j->next)
        printf("%s %s\n", j->name, j->payload);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            if (*arg1) enqueue(arg1, arg2);
        } else if (strcmp(cmd, "CLONE") == 0) {
            if (*arg1 && *arg2) clone_job(arg1, arg2);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (*arg1) cancel_job(arg1);
        } else if (strcmp(cmd, "RUN") == 0) {
            if (*arg1) run_job(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free(line);
    return 0;
}