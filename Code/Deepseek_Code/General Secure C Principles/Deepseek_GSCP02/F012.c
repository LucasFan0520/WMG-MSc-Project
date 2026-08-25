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
    if (!p) return;
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

void serve(void) {
    if (!front) return;
    Person *p = front;
    front = front->next;
    if (!front) rear = NULL;
    for (char *c = p->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
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
            if (p == rear) rear = prev;
            free(p->name);
            free(p->note);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void rejoin_person(const char *name, const char *note) {
    cancel_person(name);
    join_queue(name, note);
}

void find_person_note(const char *name) {
    Person *p = find_person(name);
    if (!p) return;
    for (char *c = p->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void print_queue(void) {
    Person *p = front;
    while (p) {
        printf("%s ", p->name);
        for (char *c = p->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        p = p->next;
    }
}

void free_all(void) {
    while (front) {
        Person *next = front->next;
        free(front->name);
        free(front->note);
        free(front);
        front = next;
    }
    rear = NULL;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "JOIN") == 0) {
            char name[256], note[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, note) == 2) {
                join_queue(name, note);
            }
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                cancel_person(name);
            }
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char name[256], note[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, note) == 2) {
                rejoin_person(name, note);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                find_person_note(name);
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }

    free(line);
    free_all();
    return 0;
}