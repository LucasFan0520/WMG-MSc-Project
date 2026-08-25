/* F012.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct person {
    char *name;
    char *note;
    struct person *next;
} Person;

Person *front = NULL, *rear = NULL;

void enqueue(const char *name, const char *note) {
    Person *p = malloc(sizeof(Person));
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (rear) rear->next = p;
    else front = p;
    rear = p;
}

Person *find_person(const char *name) {
    Person *p = front;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void serve() {
    if (!front) return;
    Person *p = front;
    front = front->next;
    if (!front) rear = NULL;
    printf("%s ", p->name);
    for (char *c = p->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
    free(p->name); free(p->note); free(p);
}

void cancel(const char *name) {
    Person *p = front, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else front = p->next;
            if (!front) rear = NULL;
            free(p->name); free(p->note); free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void rejoin(const char *name, const char *note) {
    cancel(name);  // remove existing, then add
    enqueue(name, note);
}

void find_person_print(const char *name) {
    Person *p = find_person(name);
    if (!p) { printf("NOT_FOUND\n"); return; }
    for (char *c = p->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void queue_print() {
    Person *p = front;
    while (p) {
        printf("%s ", p->name);
        for (char *c = p->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        p = p->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], note[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "SERVE") == 0) { serve(); continue; }
        if (strcmp(cmd, "QUEUE") == 0) { queue_print(); continue; }
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            if (sscanf(line, "%*s %255s", name) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            if (strcmp(cmd, "JOIN") == 0) enqueue(name, rest);
            else rejoin(name, rest);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) cancel(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) find_person_print(name);
        }
    }
    return 0;
}