/* F012.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

Person *head = NULL;
Person *tail = NULL;

Person *find_person(const char *name) {
    Person *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void free_person(Person *p) {
    free(p->name);
    free(p->note);
    free(p);
}

void join_queue(const char *name, const char *note) {
    Person *p = find_person(name);
    if (p) {
        free(p->note);
        p->note = strdup(note);
        return;
    }
    p = malloc(sizeof(Person));
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void serve(void) {
    if (!head) {
        printf("EMPTY\n");
        return;
    }
    Person *p = head;
    head = head->next;
    if (!head) tail = NULL;
    printf("%s %s\n", p->name, p->note);
    free_person(p);
}

void cancel_person(const char *name) {
    Person *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (!head) tail = NULL;
            free_person(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void rejoin(const char *name, const char *note) {
    Person *p = find_person(name);
    if (p) {
        free(p->note);
        p->note = strdup(note);
        return;
    }
    join_queue(name, note);
}

void find_person_print(const char *name) {
    Person *p = find_person(name);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->note);
}

void print_queue(void) {
    Person *p = head;
    while (p) {
        printf("%s\n", p->name);
        p = p->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256];
    char note[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "JOIN") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            join_queue(name, p);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            rejoin(name, p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) find_person_print(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            print_queue();
        }
    }

    while (head) {
        Person *tmp = head;
        head = head->next;
        free_person(tmp);
    }
    tail = NULL;
    return 0;
}