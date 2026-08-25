/*
 * F012.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct person {
    char *name;
    char *note;
    struct person *next;
} Person;

Person *front = NULL, *rear = NULL;

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
    front = front->next;
    if (!front) rear = NULL;
    printf("%s %s\n", p->name, p->note);
    free(p->name);
    free(p->note);
    free(p);
}

void cancel_person(const char *name) {
    Person *prev = NULL, *p = front;
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

void rejoin(const char *name, const char *note) {
    cancel_person(name);
    join_queue(name, note);
}

void find_person_cmd(const char *name) {
    Person *p = find_person(name);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->note);
}

void print_queue(void) {
    Person *p = front;
    while (p) {
        printf("%s\n", p->name);
        p = p->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256];
        if (sscanf(line, "%31s %255s", cmd, name) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(name) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "JOIN") == 0) {
            join_queue(name, rest);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            rejoin(name, rest);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_person_cmd(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    return 0;
}