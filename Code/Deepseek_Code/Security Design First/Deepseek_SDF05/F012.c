// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void join(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    if (!p) return;
    p->name = safe_strdup(name);
    p->note = safe_strdup(note);
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
    printf("%s\n", p->name);
    free(p->name);
    free(p->note);
    free(p);
}

void cancel(const char *name) {
    Person *prev = NULL, *cur = front;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else front = cur->next;
            if (cur == rear) rear = prev;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void rejoin(const char *name, const char *note) {
    cancel(name); // remove existing if any
    join(name, note);
}

void find_person_note(const char *name) {
    Person *p = find_person(name);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->note);
}

void print_queue(void) {
    Person *p = front;
    while (p) {
        printf("%s\n", p->name);
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            if (n < 2) continue;
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            if (strcmp(cmd, "JOIN") == 0) join(arg1, p);
            else rejoin(arg1, p);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0 && n >= 2) {
            cancel(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_person_note(arg1);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    free_all();
    return 0;
}