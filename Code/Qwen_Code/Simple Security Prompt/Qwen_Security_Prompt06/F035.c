// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

Job *jobs = NULL;
Job *tail = NULL;

Job *find_job(const char *name) {
    for (Job *j = jobs; j; j = j->next) {
        if (strcmp(j->name, name) == 0) return j;
    }
    return NULL;
}

void enqueue(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (tail) tail->next = j;
    else jobs = j;
    tail = j;
}

void clone_job(const char *old_name, const char *new_name) {
    Job *old = find_job(old_name);
    if (!old || find_job(new_name)) return;
    Job *j = malloc(sizeof(Job));
    j->name = strdup(new_name);
    j->payload = strdup(old->payload);
    j->next = NULL;
    if (tail) tail->next = j;
    else jobs = j;
    tail = j;
}

void cancel_job(const char *name) {
    Job *prev = NULL, *curr = jobs;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else jobs = curr->next;
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
    Job *prev = NULL, *curr = jobs;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else jobs = curr->next;
            if (curr == tail) tail = prev;
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
    for (Job *j = jobs; j; j = j->next) {
        printf("%s %s\n", j->name, j->payload);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0};
        sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *payload = get_rest(line, 2);
            if (payload) enqueue(arg1, payload);
        }
        else if (strcmp(cmd, "CLONE") == 0) clone_job(arg1, arg2);
        else if (strcmp(cmd, "CANCEL") == 0) cancel_job(arg1);
        else if (strcmp(cmd, "RUN") == 0) run_job(arg1);
        else if (strcmp(cmd, "LIST") == 0) list_jobs();
        
        free(line);
    }
    return 0;
}