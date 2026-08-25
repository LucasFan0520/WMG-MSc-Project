/* F012.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct person {
    char *name;
    char *note;
    struct person *next;
} person_t;

person_t *head = NULL;
person_t *tail = NULL;

person_t *find_person(const char *name) {
    person_t *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void join(const char *name, const char *note) {
    person_t *p = malloc(sizeof(person_t));
    if (!p) return;
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (!p->name || !p->note) {
        free(p->name); free(p->note); free(p);
        return;
    }
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void serve(void) {
    if (!head) return;
    person_t *p = head;
    head = head->next;
    if (!head) tail = NULL;
    printf("%s %s\n", p->name, p->note);
    free(p->name);
    free(p->note);
    free(p);
}

void cancel(const char *name) {
    person_t *prev = NULL, *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (p == tail) tail = prev;
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
    // add as new, even if duplicate
    join(name, note);
}

void find_note(const char *name) {
    person_t *p = find_person(name);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->note);
}

void queue_print(void) {
    person_t *p = head;
    while (p) {
        printf("%s %s\n", p->name, p->note);
        p = p->next;
    }
}

void free_all(void) {
    person_t *p = head;
    while (p) {
        person_t *nxt = p->next;
        free(p->name);
        free(p->note);
        free(p);
        p = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], name[256];
        if (sscanf(line, "%9s %255s", cmd, name) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "JOIN") == 0) {
                const char *note = rest + strlen(name) + 1;
                join(name, note);
            } else if (strcmp(cmd, "SERVE") == 0) {
                serve();
            } else if (strcmp(cmd, "CANCEL") == 0) {
                cancel(name);
            } else if (strcmp(cmd, "REJOIN") == 0) {
                const char *note = rest + strlen(name) + 1;
                rejoin(name, note);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_note(name);
            } else if (strcmp(cmd, "QUEUE") == 0) {
                queue_print();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}