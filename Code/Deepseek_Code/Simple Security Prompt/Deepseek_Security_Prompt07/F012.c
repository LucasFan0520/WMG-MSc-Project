// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

Person *front = NULL;
Person *rear = NULL;

Person *find_person(const char *name) {
    Person *p = front;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void join_queue(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (rear) {
        rear->next = p;
        rear = p;
    } else {
        front = rear = p;
    }
}

void serve_queue() {
    if (!front) return;
    Person *p = front;
    front = front->next;
    if (!front) rear = NULL;
    printf("%s:", p->name);
    for (char *s = p->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
    free(p->name);
    free(p->note);
    free(p);
}

void cancel_person(const char *name) {
    Person *prev = NULL;
    Person *p = front;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else front = p->next;
            if (!p->next) rear = prev;
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
    cancel_person(name);
    join_queue(name, note);
}

void find_person_note(const char *name) {
    Person *p = find_person(name);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = p->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void print_queue() {
    Person *p = front;
    int first = 1;
    while (p) {
        if (!first) putchar(' ');
        printf("%s", p->name);
        first = 0;
        p = p->next;
    }
    putchar('\n');
}

void free_all() {
    Person *p = front;
    while (p) {
        Person *next = p->next;
        free(p->name);
        free(p->note);
        free(p);
        p = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "JOIN") == 0) {
            char name[256], note[4096];
            if (sscanf(rest, "%255s %[^\n]", name, note) == 2) {
                join_queue(name, note);
            }
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve_queue();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                cancel_person(name);
            }
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char name[256], note[4096];
            if (sscanf(rest, "%255s %[^\n]", name, note) == 2) {
                rejoin_queue(name, note);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                find_person_note(name);
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    free_all();
    return 0;
}