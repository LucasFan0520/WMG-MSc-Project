// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct person {
    char *name;
    char *note;
    struct person *next;
} Person;

Person *front = NULL;
Person *rear = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Person *find_person(const char *name) {
    Person *p = front;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void join_queue(const char *name, const char *note) {
    Person *p = malloc(sizeof(Person));
    if (!p) { fprintf(stderr, "Memory error\n"); exit(1); }
    p->name = strdup(name);
    p->note = strdup(note);
    if (!p->name || !p->note) { fprintf(stderr, "Memory error\n"); exit(1); }
    p->next = NULL;
    if (rear) rear->next = p;
    else front = p;
    rear = p;
}

void serve(void) {
    if (!front) return;
    Person *p = front;
    front = front->next;
    if (!front) rear = NULL;
    for (char *s = p->note; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
    free(p->name);
    free(p->note);
    free(p);
}

void cancel(const char *name) {
    Person *p = front, *prev = NULL;
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

void rejoin(const char *name, const char *note) {
    // remove existing then add
    cancel(name);
    join_queue(name, note);
}

void find_person_note(const char *name) {
    Person *p = find_person(name);
    if (!p) return;
    for (char *s = p->note; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void print_queue(void) {
    Person *p = front;
    while (p) {
        printf("%s ", p->name);
        for (char *s = p->note; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
        }
        putchar('\n');
        p = p->next;
    }
}

void free_all(void) {
    Person *p = front;
    while (p) {
        Person *next = p->next;
        free(p->name);
        free(p->note);
        free(p);
        p = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "SERVE") == 0) { serve(); continue; }
        if (strcmp(cmd, "QUEUE") == 0) { print_queue(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char *name = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            if (strcmp(cmd, "JOIN") == 0) join_queue(name, rest);
            else rejoin(name, rest);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel(rest);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_person_note(rest);
        }
    }
    free_all();
    return 0;
}