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

Person *find_person(const char *name) {
    Person *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void join_queue(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    if (!p) exit(EXIT_FAILURE);
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (!head) {
        head = p;
        tail = p;
    } else {
        tail->next = p;
        tail = p;
    }
}

void serve_queue() {
    if (!head) return;
    Person *p = head;
    printf("%s\n", p->name);
    head = head->next;
    if (!head) tail = NULL;
    free(p->name);
    free(p->note);
    free(p);
}

void cancel_person(const char *name) {
    Person *curr = head;
    Person *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->note);
            free(curr);
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

void find_person_note(const char *name) {
    Person *p = find_person(name);
    if (!p) return;
    printf("%s\n", p->note);
}

void print_queue() {
    Person *curr = head;
    while (curr) {
        printf("%s\n", curr->name);
        curr = curr->next;
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
        
        char cmd[32] = {0};
        char name[256] = {0};
        char note[3800] = {0};
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, name, note);
        
        if (strcmp(cmd, "JOIN") == 0 && matched == 3) {
            join_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve_queue();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0 && matched == 3) {
            rejoin_queue(name, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_person_note(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    free_all();
    return 0;
}