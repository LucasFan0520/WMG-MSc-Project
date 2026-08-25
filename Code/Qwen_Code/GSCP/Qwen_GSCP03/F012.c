// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char *name, *note;
    struct Person *next;
} Person;

Person *head = NULL, *tail = NULL;

void free_list() {
    while (head) {
        Person *t = head; head = head->next;
        free(t->name); free(t->note); free(t);
    }
}

Person *find_person(const char *name) {
    for (Person *p = head; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], note[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) < 1) continue;
            Person *p = malloc(sizeof(Person));
            p->name = strdup(name); p->note = strdup(note); p->next = NULL;
            if (tail) tail->next = p; else head = p;
            tail = p;
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (head) {
                printf("%s\n", head->name);
                Person *tmp = head; head = head->next;
                if (!head) tail = NULL;
                free(tmp->name); free(tmp->note); free(tmp);
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Person *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Person *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    if (!curr) tail = prev;
                    free(tmp->name); free(tmp->note); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Person *p = find_person(name);
            if (p) printf("%s\n", p->note);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (Person *p = head; p; p = p->next) printf("%s\n", p->name);
        }
    }
    free_list();
    return 0;
}