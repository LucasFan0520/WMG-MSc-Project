// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

Person *head = NULL, *tail = NULL;

Person* find_person(const char *name) {
    for (Person *p = head; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char name[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", name, &n) != 1) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            if (find_person(name)) continue;
            Person *p = malloc(sizeof(Person));
            if (!p) continue;
            p->name = strdup(name);
            p->note = strdup(note);
            p->next = NULL;
            if (!head) head = tail = p;
            else { tail->next = p; tail = p; }
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (head) {
                Person *p = head;
                head = head->next;
                if (!head) tail = NULL;
                printf("%s\n", p->name);
                free(p->name);
                free(p->note);
                free(p);
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Person *prev = NULL;
            for (Person *p = head; p; prev = p, p = p->next) {
                if (strcmp(p->name, name) == 0) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    if (tail == p) tail = prev;
                    free(p->name);
                    free(p->note);
                    free(p);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Person *p = find_person(name);
            if (p) print_text(p->note);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (Person *p = head; p; p = p->next) printf("%s\n", p->name);
        }
    }
    while (head) {
        Person *p = head;
        head = head->next;
        free(p->name);
        free(p->note);
        free(p);
    }
    return 0;
}