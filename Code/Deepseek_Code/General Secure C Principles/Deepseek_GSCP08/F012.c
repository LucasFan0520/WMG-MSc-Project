// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person { char *name; char *note; struct Person *next; } Person;
Person *head = NULL, *tail = NULL;

Person *find_person(const char *name) {
    for (Person *p = head; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

void join(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    if (!p) return;
    p->name = strdup(name); p->note = strdup(note); p->next = NULL;
    if (tail) tail->next = p; else head = p;
    tail = p;
}

void serve(void) {
    if (!head) { printf("EMPTY\n"); return; }
    Person *p = head;
    head = head->next;
    if (!head) tail = NULL;
    printf("%s\n", p->name);
    for (char *c = p->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
    free(p->name); free(p->note); free(p);
}

void cancel(const char *name) {
    Person *prev = NULL;
    for (Person *p = head; p; prev = p, p = p->next) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next; else head = p->next;
            if (p == tail) tail = prev;
            free(p->name); free(p->note); free(p);
            return;
        }
    }
}

void rejoin(const char *name, const char *note) {
    // If already in queue, do nothing
    if (find_person(name)) return;
    join(name, note);
}

void find_person_note(const char *name) {
    Person *p = find_person(name);
    if (!p) { printf("NOT_FOUND\n"); return; }
    for (char *c = p->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void queue_print(void) {
    for (Person *p = head; p; p = p->next) printf("%s\n", p->name);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "JOIN") == 0) {
            char name[256], note[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *n = rest + strlen(name);
                while (*n == ' ') n++;
                strcpy(note, n);
                join(name, note);
            }
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) cancel(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char name[256], note[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *n = rest + strlen(name);
                while (*n == ' ') n++;
                strcpy(note, n);
                rejoin(name, note);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) find_person_note(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue_print();
        }
    }
    while (head) { Person *p = head; head = p->next; free(p->name); free(p->note); free(p); }
    return 0;
}