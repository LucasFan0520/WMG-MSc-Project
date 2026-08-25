/* F035.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
    char *name;
    char *payload;
} Job;

static Job *jobs = NULL;
static size_t jcount = 0, jcap = 0;

static Job *find_job(const char *name) {
    for (size_t i = 0; i < jcount; i++) {
        if (strcmp(jobs[i].name, name) == 0)
            return &jobs[i];
    }
    return NULL;
}

static void enqueue_job(const char *name, const char *payload) {
    if (find_job(name)) return;
    if (jcount == jcap) {
        jcap = jcap ? jcap * 2 : 4;
        jobs = realloc(jobs, jcap * sizeof(Job));
        if (!jobs) { perror("realloc"); exit(1); }
    }
    jobs[jcount].name = strdup(name);
    jobs[jcount].payload = strdup(payload);
    jcount++;
}

static void clone_job(const char *old, const char *new) {
    Job *o = find_job(old);
    if (!o || find_job(new)) return;
    enqueue_job(new, o->payload);
}

static void cancel_job(const char *name) {
    for (size_t i = 0; i < jcount; i++) {
        if (strcmp(jobs[i].name, name) == 0) {
            free(jobs[i].name);
            free(jobs[i].payload);
            for (size_t j = i; j < jcount - 1; j++)
                jobs[j] = jobs[j + 1];
            jcount--;
            return;
        }
    }
}

static void run_job(const char *name) {
    Job *j = find_job(name);
    if (!j) return;
    printf("%s\n", j->payload);
    cancel_job(name);
}

static void list_jobs(void) {
    for (size_t i = 0; i < jcount; i++) {
        printf("%s %s\n", jobs[i].name, jobs[i].payload);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *payload = strtok(NULL, "");
            if (!payload) payload = "";
            enqueue_job(name, payload);
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
    for (size_t i = 0; i < jcount; i++) {
        free(jobs[i].name);
        free(jobs[i].payload);
    }
    free(jobs);
    return 0;
}