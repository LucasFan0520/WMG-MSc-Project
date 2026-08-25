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

void join_queue(const char *name, const char *note) {
    Person *p = malloc(sizeof(Person));
    if (!p) return;
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (!head) {
        head = p;
    } else {
        Person *curr = head;
        while (curr->next) curr = curr->next;
        curr->next = p;
    }
}

void serve_queue() {
    if (!head) return;
    Person *p = head;
    head = head->next;
    printf("%s\n", p->name);
    free(p->name);
    free(p->note);
    free(p);
}

void cancel_person(const char *name) {
    Person *curr = head;
    Person *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            Person *next = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void rejoin_queue(const char *name, const char *note) {
    cancel_person(name);
    join_queue(name, note);
}

void find_person(const char *name) {
    for (Person *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            printf("%s\n", curr->note);
            return;
        }
    }
}

void print_queue() {
    for (Person *curr = head; curr; curr = curr->next) {
        printf("%s %s\n", curr->name, curr->note);
    }
}

void free_all() {
    Person *curr = head;
    while (curr) {
        Person *next = curr->next;
        free(curr->name);
        free(curr->note);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char name[1024], note[4096];
        
        int n = sscanf(line, "%15s %1023s %[^\n]", cmd, name, note);
        if (strcmp(cmd, "JOIN") == 0 && n == 3) {
            join_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve_queue();
        } else if (strcmp(cmd, "CANCEL") == 0 && n >= 2) {
            cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0 && n == 3) {
            rejoin_queue(name, note);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_person(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    free_all();
    return 0;
}