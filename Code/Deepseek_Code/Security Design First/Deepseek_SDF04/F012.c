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
    if (!p) return;
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (!p->name || !p->note) {
        free(p->name); free(p->note); free(p);
        return;
    }
    if (!head) head = tail = p;
    else {
        tail->next = p;
        tail = p;
    }
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

void cancel_person(const char *name) {
    Person *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (!head) tail = NULL;
            if (tail == p) tail = prev;
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
    // remove if exists, then add at end
    cancel_person(name);
    join_queue(name, note);
}

void find_person_note(const char *name) {
    Person *p = find_person(name);
    if (!p) return;
    printf("%s\n", p->note);
}

void queue_list(void) {
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
    head = tail = NULL;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], note[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "JOIN") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) == 2)
                join_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) == 2)
                rejoin_queue(name, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_person_note(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue_list();
        }
    }
    free(line);
    free_all();
    return 0;
}