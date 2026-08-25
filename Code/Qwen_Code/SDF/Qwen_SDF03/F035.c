// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

Job *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Job *find_job(const char *name) {
    for (Job *j = head; j; j = j->next)
        if (strcmp(j->name, name) == 0) return j;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *name = get_token(&ptr);
            if (name && !find_job(name)) {
                Job *j = malloc(sizeof(Job));
                if (j) {
                    j->name = strdup(name);
                    j->payload = strdup(ptr);
                    j->next = head;
                    head = j;
                }
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *old = get_token(&ptr);
            char *newn = get_token(&ptr);
            if (old && newn && !find_job(newn)) {
                Job *j = find_job(old);
                if (j) {
                    Job *nj = malloc(sizeof(Job));
                    if (nj) {
                        nj->name = strdup(newn);
                        nj->payload = strdup(j->payload);
                        nj->next = head;
                        head = nj;
                    }
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Job *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free(curr->name);
                        free(curr->payload);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Job *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        printf("%s\n", curr->payload);
                        free(curr->name);
                        free(curr->payload);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Job *j = head; j; j = j->next) {
                printf("%s %s\n", j->name, j->payload);
            }
        }
    }
    while (head) {
        Job *next = head->next;
        free(head->name);
        free(head->payload);
        free(head);
        head = next;
    }
    return 0;
}