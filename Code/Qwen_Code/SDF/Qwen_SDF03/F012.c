// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char *name, *note;
    struct Person *next;
} Person;

Person *head = NULL, *tail = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Person *find_person(const char *name) {
    for (Person *p = head; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

void enqueue(const char *name, const char *note) {
    Person *p = malloc(sizeof(Person));
    if (!p) return;
    p->name = strdup(name); p->note = strdup(note); p->next = NULL;
    if (!p->name || !p->note) { free(p->name); free(p->note); free(p); return; }
    if (tail) tail->next = p; else head = p;
    tail = p;
}

void remove_person(Person *prev, Person *curr) {
    if (prev) prev->next = curr->next; else head = curr->next;
    if (tail == curr) tail = prev;
    free(curr->name); free(curr->note); free(curr);
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char *name = get_token(&ptr);
            if (name && !find_person(name)) enqueue(name, ptr);
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (head) {
                printf("%s\n", head->name);
                remove_person(NULL, head);
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Person *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) { remove_person(prev, curr); break; }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Person *p = find_person(name);
                if (p) printf("%s\n", p->note);
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (Person *p = head; p; p = p->next) printf("%s %s\n", p->name, p->note);
        }
    }
    while (head) {
        Person *next = head->next;
        free(head->name); free(head->note); free(head);
        head = next;
    }
    return 0;
}