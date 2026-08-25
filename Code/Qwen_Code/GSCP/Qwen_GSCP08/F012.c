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

Person *find_person(char *name) {
    for (Person *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) return p;
    }
    return NULL;
}

void join_queue(char *name, char *note) {
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (!head) {
        head = p;
    } else {
        Person *last = head;
        while (last->next) last = last->next;
        last->next = p;
    }
}

void serve_queue() {
    if (head) {
        printf("%s\n", head->name);
        Person *next = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = next;
    }
}

void cancel_person(char *name) {
    Person *prev = NULL;
    Person *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void rejoin_queue(char *name, char *note) {
    cancel_person(name);
    join_queue(name, note);
}

void find_person_note(char *name) {
    Person *p = find_person(name);
    if (p) puts(p->note);
}

void print_queue() {
    for (Person *p = head; p; p = p->next) {
        puts(p->name);
    }
}

int main() {
    char cmd[20], name[100], note[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            scanf("%99s", name);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            if (strcmp(cmd, "JOIN") == 0) join_queue(name, note);
            else rejoin_queue(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve_queue();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            scanf("%99s", name);
            cancel_person(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", name);
            find_person_note(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }
    return 0;
}