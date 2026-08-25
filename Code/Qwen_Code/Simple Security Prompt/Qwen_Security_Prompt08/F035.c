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

Job* find_job(char *name) {
    for (Job *j = head; j; j = j->next) {
        if (!strcmp(j->name, name)) return j;
    }
    return NULL;
}

int main() {
    char cmd[20];
    char name[100];
    char payload[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ENQUEUE")) {
            scanf("%s", name);
            fgets(payload, 1000, stdin);
            payload[strcspn(payload, "\n")] = 0;
            if (payload[0] == ' ') memmove(payload, payload + 1, strlen(payload));
            Job *j = malloc(sizeof(Job));
            j->name = strdup(name);
            j->payload = strdup(payload);
            j->next = NULL;
            if (!head) head = j;
            else {
                Job *p = head;
                while (p->next) p = p->next;
                p->next = j;
            }
        } else if (!strcmp(cmd, "CLONE")) {
            char oldName[100], newName[100];
            scanf("%s %s", oldName, newName);
            Job *old = find_job(oldName);
            if (old) {
                Job *j = malloc(sizeof(Job));
                j->name = strdup(newName);
                j->payload = strdup(old->payload);
                j->next = NULL;
                if (!head) head = j;
                else {
                    Job *p = head;
                    while (p->next) p = p->next;
                    p->next = j;
                }
            }
        } else if (!strcmp(cmd, "CANCEL")) {
            scanf("%s", name);
            Job *p = head;
            Job *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    free(p->name);
                    free(p->payload);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "RUN")) {
            scanf("%s", name);
            Job *p = head;
            Job *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    printf("%s\n", p->payload);
                    free(p->name);
                    free(p->payload);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (Job *j = head; j; j = j->next) {
                printf("%s %s\n", j->name, j->payload);
            }
        }
    }
    return 0;
}