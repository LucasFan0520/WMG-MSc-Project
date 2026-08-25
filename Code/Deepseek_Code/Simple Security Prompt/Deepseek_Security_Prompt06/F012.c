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
    p->name = malloc(strlen(name) + 1);
    strcpy(p->name, name);
    p->note = malloc(strlen(note) + 1);
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
    // remove if exists, then add at tail
    Person *p = find_person(name);
    if (p) cancel_person(name);
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
        Person *tmp = p->next;
        free(p->name);
        free(p->note);
        free(p);
        p = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], note[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "JOIN") == 0) {
            if (sscanf(rest, "%255s", name) == 1) {
                const char *n = rest + strlen(name) + 1;
                while (*n == ' ') n++;
                if (*n) join_queue(name, n);
            }
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(rest, "%255s", name) == 1) cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            if (sscanf(rest, "%255s", name) == 1) {
                const char *n = rest + strlen(name) + 1;
                while (*n == ' ') n++;
                if (*n) rejoin_queue(name, n);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", name) == 1) find_person_note(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue_list();
        }
    }
    free_all();
    return 0;
}