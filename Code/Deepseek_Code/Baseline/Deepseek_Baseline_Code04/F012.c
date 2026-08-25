/* F012.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

Person *front = NULL, *rear = NULL;

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
    printf("%s %s\n", p->name, p->note);
    front = front->next;
    if (!front) rear = NULL;
    free(p->name);
    free(p->note);
    free(p);
}

void cancel_person(const char *name) {
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
    // remove if exists, then add again
    cancel_person(name);
    join_queue(name, note);
}

void find_person_print(const char *name) {
    Person *p = find_person(name);
    if (p) printf("%s\n", p->note);
}

void print_queue(void) {
    Person *p = front;
    while (p) {
        printf("%s\n", p->name);
        p = p->next;
    }
}

void free_queue(void) {
    while (front) {
        Person *tmp = front;
        front = front->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
    rear = NULL;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256];
        int n = sscanf(line, "%s %s", cmd, name);
        if (strcmp(cmd, "JOIN") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            if (*p) join_queue(name, p);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (n < 2) continue;
            cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            if (*p) rejoin(name, p);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            find_person_print(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    free_queue();
    return 0;
}