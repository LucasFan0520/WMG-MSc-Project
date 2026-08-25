/* F012.c */
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

Person *find_person(char *name) {
    for (Person *p = front; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

void join_queue(char *name, char *note) {
    Person *p = find_person(name);
    if (p) return; // already in queue? not specified, but we can ignore.
    Person *new = malloc(sizeof(Person));
    new->name = strdup(name);
    new->note = strdup(note);
    new->next = NULL;
    if (!rear) front = rear = new;
    else { rear->next = new; rear = new; }
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

void cancel(char *name) {
    Person *prev = NULL;
    for (Person *p = front; p; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else front = p->next;
            if (p == rear) rear = prev;
            free(p->name);
            free(p->note);
            free(p);
            return;
        }
        prev = p;
    }
}

void rejoin(char *name, char *note) {
    // remove existing, then add
    cancel(name);
    join_queue(name, note);
}

void find_person_note(char *name) {
    Person *p = find_person(name);
    if (p) printf("%s\n", p->note);
}

void queue_all(void) {
    for (Person *p = front; p; p = p->next)
        printf("%s %s\n", p->name, p->note);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "JOIN") == 0 && n == 3) {
            join_queue(arg1, arg2);
        } else if (strcmp(cmd, "SERVE") == 0 && n == 1) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0 && n == 2) {
            cancel(arg1);
        } else if (strcmp(cmd, "REJOIN") == 0 && n == 3) {
            rejoin(arg1, arg2);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_person_note(arg1);
        } else if (strcmp(cmd, "QUEUE") == 0 && n == 1) {
            queue_all();
        }
    }
    free(line);
    free_all();
    return 0;
}