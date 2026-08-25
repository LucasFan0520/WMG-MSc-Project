// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct person {
    char *name;
    char *note;
    struct person *next;
} Person;

Person *front = NULL;
Person *rear = NULL;

Person *find_person(const char *name) {
    Person *p = front;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void join_queue(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (rear) rear->next = p;
    else front = p;
    rear = p;
}

void serve(void) {
    if (!front) return;
    Person *p = front;
    front = p->next;
    if (!front) rear = NULL;
    printf("%s %s\n", p->name, p->note);
    free(p->name);
    free(p->note);
    free(p);
}

void cancel_person(const char *name) {
    Person *p = front, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else front = p->next;
            if (p == rear) rear = prev;
            free(p->name);
            free(p->note);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void rejoin_queue(const char *name, const char *note) {
    cancel_person(name);
    join_queue(name, note);
}

void find_person_print(const char *name) {
    Person *p = find_person(name);
    if (p) printf("%s\n", p->note);
}

void queue_print(void) {
    Person *p = front;
    while (p) {
        printf("%s %s\n", p->name, p->note);
        p = p->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], note[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "JOIN") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", name, note) == 2)
                join_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", name, note) == 2)
                rejoin_queue(name, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_person_print(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue_print();
        }
    }
    return 0;
}