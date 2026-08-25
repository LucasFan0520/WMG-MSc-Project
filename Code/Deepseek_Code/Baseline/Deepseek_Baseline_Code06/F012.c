/* F012.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

Person *front = NULL;
Person *rear = NULL;

Person* find_person(const char *name) {
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

void serve() {
    if (!front) return;
    Person *p = front;
    front = front->next;
    if (!front) rear = NULL;
    printf("%s %s\n", p->name, p->note);
    free(p->name);
    free(p->note);
    free(p);
}

void cancel(const char *name) {
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

void rejoin(const char *name, const char *note) {
    cancel(name); // remove if exists
    join_queue(name, note);
}

void find_name(const char *name) {
    Person *p = find_person(name);
    if (!p) return;
    printf("%s\n", p->note);
}

void queue_print() {
    Person *p = front;
    while (p) {
        printf("%s %s\n", p->name, p->note);
        p = p->next;
    }
}

void free_all() {
    while (front) {
        Person *tmp = front;
        front = front->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
    rear = NULL;
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char name[256]; char note[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, name, note);
        if (strcmp(cmd, "JOIN") == 0 && n == 3) {
            join_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0 && n == 1) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0 && n == 2) {
            cancel(name);
        } else if (strcmp(cmd, "REJOIN") == 0 && n == 3) {
            rejoin(name, note);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_name(name);
        } else if (strcmp(cmd, "QUEUE") == 0 && n == 1) {
            queue_print();
        }
    }
    free_all();
    return 0;
}