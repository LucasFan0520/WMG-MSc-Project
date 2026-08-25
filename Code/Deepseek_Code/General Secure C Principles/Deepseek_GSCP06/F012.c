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
    if (!front) front = rear = p;
    else {
        rear->next = p;
        rear = p;
    }
}

void serve(void) {
    if (!front) return;
    Person *p = front;
    front = front->next;
    if (!front) rear = NULL;
    printf("%s %s\n", p->name, p->note);
    free(p->name);
    free(p->note);
    free(p);
}

void cancel(const char *name) {
    Person *cur = front, *prev = NULL;
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
    // remove if exists, then add at rear
    cancel(name);
    join_queue(name, note);
}

void find_person_cmd(const char *name) {
    Person *p = find_person(name);
    if (p) printf("%s\n", p->note);
}

void print_queue(void) {
    Person *p = front;
    while (p) {
        printf("%s %s\n", p->name, p->note);
        p = p->next;
    }
}

void cleanup_queue(void) {
    Person *p = front;
    while (p) {
        Person *tmp = p;
        p = p->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], rest[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, name, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "JOIN") == 0) {
            char *note = line + strlen(cmd) + 1 + strlen(name) + 1;
            join_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char *note = line + strlen(cmd) + 1 + strlen(name) + 1;
            rejoin(name, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_person_cmd(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    cleanup_queue();
    return 0;
}