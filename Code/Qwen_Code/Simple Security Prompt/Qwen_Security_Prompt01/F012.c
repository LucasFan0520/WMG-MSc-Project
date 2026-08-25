// F012.c
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

void join_queue(const char *n, const char *nt) {
    Person *p = malloc(sizeof(Person));
    p->name = strdup(n);
    p->note = strdup(nt);
    p->next = NULL;
    if (!head) {
        head = tail = p;
    } else {
        tail->next = p;
        tail = p;
    }
}

void serve_queue() {
    if (!head) {
        return;
    }
    printf("%s\n", head->name);
    Person *p = head;
    head = head->next;
    if (!head) {
        tail = NULL;
    }
    free(p->name);
    free(p->note);
    free(p);
}

void cancel_queue(const char *n) {
    Person *p = head;
    Person *prev = NULL;
    while (p) {
        if (strcmp(p->name, n) == 0) {
            Person *next = p->next;
            free(p->name);
            free(p->note);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            if (tail == p) {
                tail = prev;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void rejoin_queue(const char *n, const char *nt) {
    cancel_queue(n);
    join_queue(n, nt);
}

void find_queue(const char *n) {
    for (Person *p = head; p; p = p->next) {
        if (strcmp(p->name, n) == 0) {
            printf("%s\n", p->note);
            return;
        }
    }
}

void print_queue() {
    for (Person *p = head; p; p = p->next) {
        printf("%s %s\n", p->name, p->note);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char name[1000];
        char note[8000];
        note[0] = 0;
        sscanf(line, "%s %s %[^\n]", cmd, name, note);
        if (strcmp(cmd, "JOIN") == 0) {
            join_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve_queue();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel_queue(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            rejoin_queue(name, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_queue(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    while (head) {
        Person *n = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = n;
    }
    return 0;
}