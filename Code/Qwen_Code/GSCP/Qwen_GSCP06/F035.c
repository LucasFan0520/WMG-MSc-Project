// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

typedef struct Job {
    char* name;
    char* payload;
    struct Job* next;
} Job;

Job* head = NULL;

Job* find_job(const char* name) {
    for (Job* j = head; j; j = j->next)
        if (strcmp(j->name, name) == 0) return j;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "ENQUEUE") == 0) {
            char* name = get_word(&p);
            if (name) {
                Job* n = malloc(sizeof(Job));
                n->name = strdup(name);
                n->payload = strdup(p);
                n->next = NULL;
                if (!head) head = n;
                else {
                    Job* tail = head;
                    while (tail->next) tail = tail->next;
                    tail->next = n;
                }
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char* old = get_word(&p);
            char* new_name = get_word(&p);
            Job* j = find_job(old);
            if (j && new_name && !find_job(new_name)) {
                Job* n = malloc(sizeof(Job));
                n->name = strdup(new_name);
                n->payload = strdup(j->payload);
                n->next = NULL;
                Job* tail = head;
                while (tail->next) tail = tail->next;
                tail->next = n;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char* name = get_word(&p);
            Job* prev = NULL;
            for (Job* j = head; j; prev = j, j = j->next) {
                if (strcmp(j->name, name) == 0) {
                    if (prev) prev->next = j->next;
                    else head = j->next;
                    free(j->name);
                    free(j->payload);
                    free(j);
                    break;
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char* name = get_word(&p);
            Job* prev = NULL;
            for (Job* j = head; j; prev = j, j = j->next) {
                if (strcmp(j->name, name) == 0) {
                    printf("%s\n", j->payload);
                    if (prev) prev->next = j->next;
                    else head = j->next;
                    free(j->name);
                    free(j->payload);
                    free(j);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Job* j = head; j; j = j->next) {
                printf("%s %s\n", j->name, j->payload);
            }
        }
    }
    return 0;
}