// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct person {
    char *name;
    char *note;
    struct person *next;
};

struct person *queue_head = NULL;
struct person *queue_tail = NULL;

struct person *find_person(char *name) {
    struct person *p = queue_head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void join_queue(char *name, char *note) {
    struct person *p = find_person(name);
    if (p) return;
    p = malloc(sizeof(struct person));
    p->name = malloc(strlen(name) + 1);
    strcpy(p->name, name);
    p->note = malloc(strlen(note) + 1);
    strcpy(p->note, note);
    p->next = NULL;
    if (queue_tail) {
        queue_tail->next = p;
        queue_tail = p;
    } else {
        queue_head = queue_tail = p;
    }
}

void serve() {
    if (!queue_head) return;
    struct person *p = queue_head;
    queue_head = queue_head->next;
    if (!queue_head) queue_tail = NULL;
    printf("%s %s\n", p->name, p->note);
    free(p->name);
    free(p->note);
    free(p);
}

void cancel_person(char *name) {
    struct person *p = queue_head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else queue_head = p->next;
            if (queue_tail == p) queue_tail = prev;
            free(p->name);
            free(p->note);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void rejoin_queue(char *name, char *note) {
    cancel_person(name);
    join_queue(name, note);
}

void find_person_print(char *name) {
    struct person *p = find_person(name);
    if (p) printf("%s\n", p->note);
}

void print_queue() {
    struct person *p = queue_head;
    while (p) {
        printf("%s %s\n", p->name, p->note);
        p = p->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], note[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, name, note);
        if (strcmp(cmd, "JOIN") == 0 && n >= 3) {
            join_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0 && n >= 2) {
            cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0 && n >= 3) {
            rejoin_queue(name, note);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_person_print(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    return 0;
}