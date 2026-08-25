// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person { char *name, *note; struct Person *next; } Person;
Person *head = NULL, *tail = NULL;

void join_queue(const char *name, const char *note) {
    Person *p = malloc(sizeof(Person));
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (!head) head = tail = p;
    else { tail->next = p; tail = p; }
}

void serve_queue() {
    if (head) {
        printf("%s\n", head->name);
        Person *tmp = head;
        head = head->next;
        if (!head) tail = NULL;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

void cancel_queue(const char *name) {
    Person *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void rejoin_queue(const char *name, const char *note) { join_queue(name, note); }

void find_person(const char *name) {
    for (Person *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) { printf("%s\n", p->note); return; }
    }
}

void print_queue() {
    for (Person *p = head; p; p = p->next) printf("%s\n", p->name);
}

void cleanup() {
    while (head) {
        Person *tmp = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], note[8192] = "";
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) strcpy(note, t + 1); }
                join_queue(name, note);
            }
        } else if (strcmp(cmd, "SERVE") == 0) serve_queue();
        else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) cancel_queue(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) find_person(name);
        } else if (strcmp(cmd, "QUEUE") == 0) print_queue();
    }
    cleanup();
    return 0;
}