// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_NOTE 4096

typedef struct Person {
    char name[MAX_NAME];
    char note[MAX_NOTE];
    struct Person *next;
} Person;

Person *head = NULL;
Person *tail = NULL;

Person *find_person(const char *name) {
    Person *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void free_all() {
    Person *p = head;
    while (p) {
        Person *tmp = p;
        p = p->next;
        free(tmp);
    }
    head = tail = NULL;
}

void handle_join(char *line) {
    char name[MAX_NAME], note[MAX_NOTE];
    int n = sscanf(line, "JOIN %63s %4095[^\n]", name, note);
    if (n < 2) return;
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    if (!p) return;
    strcpy(p->name, name);
    strcpy(p->note, note);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void handle_serve() {
    if (!head) return;
    Person *p = head;
    head = head->next;
    if (!head) tail = NULL;
    printf("%s %s\n", p->name, p->note);
    free(p);
}

void handle_cancel(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "CANCEL %63s", name) != 1) return;
    Person *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (!head) tail = NULL;
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void handle_rejoin(char *line) {
    char name[MAX_NAME], note[MAX_NOTE];
    int n = sscanf(line, "REJOIN %63s %4095[^\n]", name, note);
    if (n < 2) return;
    // Remove if exists
    Person *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (!head) tail = NULL;
            free(p);
            break;
        }
        prev = p;
        p = p->next;
    }
    // Add at tail
    Person *newp = malloc(sizeof(Person));
    if (!newp) return;
    strcpy(newp->name, name);
    strcpy(newp->note, note);
    newp->next = NULL;
    if (tail) tail->next = newp;
    else head = newp;
    tail = newp;
}

void handle_find(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "FIND %63s", name) != 1) return;
    Person *p = find_person(name);
    if (!p) return;
    printf("%s\n", p->note);
}

void handle_queue() {
    Person *p = head;
    while (p) {
        printf("%s %s\n", p->name, p->note);
        p = p->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "JOIN ", 5) == 0) handle_join(line);
        else if (strcmp(line, "SERVE") == 0) handle_serve();
        else if (strncmp(line, "CANCEL ", 7) == 0) handle_cancel(line);
        else if (strncmp(line, "REJOIN ", 7) == 0) handle_rejoin(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "QUEUE") == 0) handle_queue();
    }
    free_all();
    return 0;
}