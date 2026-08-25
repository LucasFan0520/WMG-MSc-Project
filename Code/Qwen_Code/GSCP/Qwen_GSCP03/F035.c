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

void free_list() {
    while (head) {
        Job *j = head; head = head->next;
        free(j->name); free(j->payload); free(j);
    }
}

Job *find_job(const char *name) {
    for (Job *j = head; j; j = j->next)
        if (strcmp(j->name, name) == 0) return j;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], name2[256], payload[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ENQUEUE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, payload) < 1) continue;
            if (find_job(name)) continue;
            Job *j = malloc(sizeof(Job));
            j->name = strdup(name); j->payload = strdup(payload);
            j->next = head; head = j;
        } else if (strcmp(cmd, "CLONE") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, name2) != 2) continue;
            Job *src = find_job(name);
            if (src && !find_job(name2)) {
                Job *j = malloc(sizeof(Job));
                j->name = strdup(name2); j->payload = strdup(src->payload);
                j->next = head; head = j;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Job *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Job *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->payload); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Job *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Job *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    printf("%s %s\n", tmp->name, tmp->payload);
                    free(tmp->name); free(tmp->payload); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Job *j = head; j; j = j->next) {
                printf("%s %s\n", j->name, j->payload);
            }
        }
    }
    free_list();
    return 0;
}