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
    Person *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void join(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *new = malloc(sizeof(Person));
    new->name = strdup(name);
    new->note = strdup(note);
    new->next = NULL;
    if (!tail) head = tail = new;
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
    free(p->name);
    free(p->note);
    free(p);
}

void cancel(const char *name) {
    Person *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            if (cur == tail) tail = prev;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void rejoin(const char *name, const char *note) {
    cancel(name); // remove if exists
    join(name, note);
}

void find(const char *name) {
    Person *p = find_person(name);
    if (p) printf("%s\n", p->note);
}

void print_queue() {
    Person *cur = head;
    while (cur) {
        printf("%s %s\n", cur->name, cur->note);
        cur = cur->next;
    }
}

void free_all() {
    Person *cur = head;
    while (cur) {
        Person *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "JOIN") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            join(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = strtok(NULL, " ");
            if (name) cancel(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            rejoin(name, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    free_all();
    return 0;
}