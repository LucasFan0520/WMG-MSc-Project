// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

Person *head = NULL;
Person *tail = NULL;

Person* find_person(const char *name) {
    Person *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void free_person(Person *p) {
    free(p->name);
    free(p->note);
    free(p);
}

void join(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *new = malloc(sizeof(Person));
    new->name = strdup(name);
    new->note = strdup(note);
    new->next = NULL;
    if (!head) head = tail = new;
    else {
        tail->next = new;
        tail = new;
    }
}

void serve() {
    if (!head) return;
    Person *p = head;
    head = head->next;
    if (!head) tail = NULL;
    printf("%s %s\n", p->name, p->note);
    free_person(p);
}

void cancel(const char *name) {
    Person *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (!head) tail = NULL;
            free_person(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void rejoin(const char *name, const char *note) {
    // Remove if exists, then add to end
    cancel(name);
    join(name, note);
}

void find_person_print(const char *name) {
    Person *p = find_person(name);
    if (!p) return;
    printf("%s\n", p->note);
}

void queue() {
    Person *p = head;
    while (p) {
        printf("%s %s\n", p->name, p->note);
        p = p->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "JOIN") == 0) {
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name) {
                if (!note) note = "";
                join(name, note);
            }
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = strtok(NULL, " ");
            if (name) cancel(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name) {
                if (!note) note = "";
                rejoin(name, note);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_person_print(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue();
        }
    }
    while (head) {
        Person *tmp = head;
        head = head->next;
        free_person(tmp);
    }
    tail = NULL;
    return 0;
}