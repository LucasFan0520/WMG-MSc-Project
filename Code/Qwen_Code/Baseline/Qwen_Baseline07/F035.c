// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
    char *name, *payload;
    struct Job *next;
} Job;

Job *head = NULL;

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

Job *find_job(const char *name) {
    for (Job *j = head; j; j = j->next) if (strcmp(j->name, name) == 0) return j;
    return NULL;
}

int main() {
    char cmd[20], n1[256], n2[256], payload[2048];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ENQUEUE") == 0) {
            scanf("%s", n1); read_rest(payload, sizeof(payload));
            Job *j = malloc(sizeof(Job));
            j->name = strdup(n1); j->payload = strdup(payload); j->next = head; head = j;
        } else if (strcmp(cmd, "CLONE") == 0) {
            scanf("%s %s", n1, n2);
            Job *src = find_job(n1);
            if (src) {
                Job *j = malloc(sizeof(Job));
                j->name = strdup(n2); j->payload = strdup(src->payload); j->next = head; head = j;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            scanf("%s", n1);
            Job *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, n1) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->name); free(curr->payload); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            scanf("%s", n1);
            Job *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, n1) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    printf("%s %s\n", curr->name, curr->payload);
                    free(curr->name); free(curr->payload); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Job *j = head; j; j = j->next) printf("%s %s\n", j->name, j->payload);
        }
    }
    return 0;
}