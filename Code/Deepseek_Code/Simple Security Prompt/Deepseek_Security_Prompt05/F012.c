/* F012.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

Person *head = NULL, *tail = NULL;

Person *find_person(const char *name) {
    Person *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void enqueue(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    if (!p) return;
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void serve(void) {
    if (!head) return;
    Person *p = head;
    head = head->next;
    if (!head) tail = NULL;
    printf("%s %s\n", p->name, p->note);
    free(p->name);
    free(p->note);
    free(p);
}

void cancel(const char *name) {
    Person *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (!head) tail = NULL;
            free(p->name);
            free(p->note);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void rejoin(const char *name, const char *note) {
    cancel(name);
    enqueue(name, note);
}

void find_person_note(const char *name) {
    Person *p = find_person(name);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->note);
}

void queue_all(void) {
    Person *p = head;
    while (p) {
        printf("%s %s\n", p->name, p->note);
        p = p->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "JOIN") == 0) {
            char *note = line + strlen(cmd) + strlen(arg1) + 2;
            while (*note == ' ') note++;
            enqueue(arg1, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            cancel(arg1);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char *note = line + strlen(cmd) + strlen(arg1) + 2;
            while (*note == ' ') note++;
            rejoin(arg1, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_person_note(arg1);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue_all();
        }
    }
    while (head) {
        Person *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
    return 0;
}