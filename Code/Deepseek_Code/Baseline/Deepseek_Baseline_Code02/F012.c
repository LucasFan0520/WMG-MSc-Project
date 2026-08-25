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
    p->name = malloc(strlen(name) + 1);
    strcpy(p->name, name);
    p->note = malloc(strlen(note) + 1);
    strcpy(p->note, note);
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
    printf("%s ", p->name);
    for (char *c = p->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
    free(p->name);
    free(p->note);
    free(p);
}

void cancel(const char *name) {
    Person *prev = NULL;
    Person *p = front;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else front = p->next;
            if (rear == p) rear = prev;
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
    cancel(name); // remove if exists, then add again
    join_queue(name, note);
}

void find_person_print(const char *name) {
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "JOIN") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            join_queue(name, p);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            cancel(p);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            rejoin(name, p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_person_print(p);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    free_all();
    return 0;
}