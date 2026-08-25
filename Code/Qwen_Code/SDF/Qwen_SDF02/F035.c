// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

Job *jobs = NULL;

Job* find_job(const char *name) {
    for (Job *j = jobs; j; j = j->next)
        if (strcmp(j->name, name) == 0) return j;
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", name, &n) != 1) continue;
            char *payload = rest + n;
            while (*payload == ' ') payload++;
            if (find_job(name)) continue;
            Job *j = malloc(sizeof(Job));
            if (!j) continue;
            j->name = strdup(name);
            j->payload = strdup(payload);
            if (!j->name || !j->payload) { free(j->name); free(j->payload); free(j); continue; }
            j->next = jobs;
            jobs = j;
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old_name[256], new_name[256];
            if (sscanf(rest, "%255s %255s", old_name, new_name) != 2) continue;
            Job *old = find_job(old_name);
            if (old && !find_job(new_name)) {
                Job *j = malloc(sizeof(Job));
                if (!j) continue;
                j->name = strdup(new_name);
                j->payload = strdup(old->payload);
                if (!j->name || !j->payload) { free(j->name); free(j->payload); free(j); continue; }
                j->next = jobs;
                jobs = j;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Job *prev = NULL;
            for (Job *j = jobs; j; prev = j, j = j->next) {
                if (strcmp(j->name, name) == 0) {
                    if (prev) prev->next = j->next;
                    else jobs = j->next;
                    free(j->name); free(j->payload); free(j);
                    break;
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Job *prev = NULL;
            for (Job *j = jobs; j; prev = j, j = j->next) {
                if (strcmp(j->name, name) == 0) {
                    if (prev) prev->next = j->next;
                    else jobs = j->next;
                    printf("%s ", j->name);
                    print_text(j->payload);
                    free(j->name); free(j->payload); free(j);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Job *j = jobs; j; j = j->next) {
                printf("%s ", j->name);
                print_text(j->payload);
            }
        }
    }
    while (jobs) {
        Job *j = jobs;
        jobs = jobs->next;
        free(j->name); free(j->payload); free(j);
    }
    return 0;
}