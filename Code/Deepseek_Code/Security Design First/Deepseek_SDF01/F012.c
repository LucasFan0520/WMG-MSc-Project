// filename: F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

static Person *head = NULL;
static Person *tail = NULL;

static void cleanup(void) {
    while (head) {
        Person *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

static Person *find_person(const char *name) {
    for (Person *p = head; p; p = p->next)
        if (strcmp(p->name, name) == 0)
            return p;
    return NULL;
}

static void enqueue(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    if (!p) return;
    p->name = strdup(name);
    p->note = strdup(note);
    if (!p->name || !p->note) {
        free(p->name);
        free(p->note);
        free(p);
        return;
    }
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

static void serve(void) {
    if (!head) return;
    Person *p = head;
    head = head->next;
    if (!head) tail = NULL;
    printf("%s %s\n", p->name, p->note);
    free(p->name);
    free(p->note);
    free(p);
}

static void cancel(const char *name) {
    Person *prev = NULL;
    for (Person *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (!head) tail = NULL;
            free(p->name);
            free(p->note);
            free(p);
            return;
        }
        prev = p;
    }
}

static void rejoin(const char *name, const char *note) {
    cancel(name); // remove if exists, then enqueue new
    enqueue(name, note);
}

static void find_print(const char *name) {
    Person *p = find_person(name);
    if (p) printf("%s\n", p->note);
}

static void queue_print(void) {
    for (Person *p = head; p; p = p->next)
        printf("%s %s\n", p->name, p->note);
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "JOIN") == 0) {
            if (*arg1) enqueue(arg1, arg2);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (*arg1) cancel(arg1);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            if (*arg1) rejoin(arg1, arg2);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_print(arg1);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue_print();
        }
    }
    free(line);
    return 0;
}