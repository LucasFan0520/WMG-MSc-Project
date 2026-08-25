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
    Person *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void join_queue(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    p->name = malloc(strlen(name)+1);
    strcpy(p->name, name);
    p->note = malloc(strlen(note)+1);
    strcpy(p->note, note);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void serve(void) {
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
    Person *prev = NULL;
    Person *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (p == tail) tail = prev;
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
    cancel(name);
    join_queue(name, note);
}

void find_person_print(const char *name) {
    Person *p = find_person(name);
    if (!p) return;
    printf("%s\n", p->note);
}

void queue_print(void) {
    Person *p = head;
    while (p) {
        printf("%s %s\n", p->name, p->note);
        p = p->next;
    }
}

void free_all(void) {
    Person *p = head;
    while (p) {
        Person *next = p->next;
        free(p->name);
        free(p->note);
        free(p);
        p = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "JOIN") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            join_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = strtok(rest, " ");
            if (name) cancel(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            rejoin(name, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(rest, " ");
            if (name) find_person_print(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue_print();
        }
    }
    free_all();
    return 0;
}