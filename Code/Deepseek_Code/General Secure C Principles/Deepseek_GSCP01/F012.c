/* F012.c */
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
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
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
    printf("%s ", p->name);
    for (char *c = p->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
    free(p->name);
    free(p->note);
    free(p);
}

void cancel_person(const char *name) {
    Person *p = head, *prev = NULL;
    while (p && strcmp(p->name, name) != 0) {
        prev = p;
        p = p->next;
    }
    if (!p) return;
    if (prev) prev->next = p->next;
    else head = p->next;
    if (tail == p) tail = prev;
    free(p->name);
    free(p->note);
    free(p);
}

void rejoin(const char *name, const char *note) {
    cancel_person(name);
    join_queue(name, note);
}

void find_person_note(const char *name) {
    Person *p = find_person(name);
    if (!p) { printf("NOT_FOUND\n"); return; }
    for (char *c = p->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void print_queue(void) {
    Person *p = head;
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "JOIN") == 0) {
            char name[256];
            char *p = line + 5;
            if (sscanf(p, "%255s", name) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            join_queue(name, p);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            cancel_person(p);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char name[256];
            char *p = line + 7;
            if (sscanf(p, "%255s", name) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            rejoin(name, p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_person_note(p);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    free_all();
    free(line);
    return 0;
}