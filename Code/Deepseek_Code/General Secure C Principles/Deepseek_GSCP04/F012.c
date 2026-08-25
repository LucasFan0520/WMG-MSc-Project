/* F012.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    char *name;
    char *note;
    struct Person *next;
} Person;

static Person *head = NULL, *tail = NULL;

static Person *find_person(const char *name) {
    Person *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

static void join_person(const char *name, const char *note) {
    Person *p = malloc(sizeof(Person));
    if (!p) { perror("malloc"); exit(1); }
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

static void serve_person(void) {
    if (!head) return;
    Person *p = head;
    head = head->next;
    if (!head) tail = NULL;
    printf("%s %s\n", p->name, p->note);
    free(p->name);
    free(p->note);
    free(p);
}

static void cancel_person(const char *name) {
    Person **curr = &head;
    while (*curr) {
        if (strcmp((*curr)->name, name) == 0) {
            Person *p = *curr;
            *curr = p->next;
            if (!*curr) tail = NULL; // if we removed last, update tail? Actually we need to track tail properly.
            // But if we remove the tail, we need to find new tail? Simpler: we can just update tail if we removed the last node.
            // We'll handle by checking if p == tail then set tail to previous node? Not trivial with singly.
            // We'll just leave tail as is; but if tail removed, tail might point to freed. We'll set tail to NULL if list becomes empty.
            if (p == tail) {
                // find new tail
                if (head == NULL) tail = NULL;
                else {
                    Person *t = head;
                    while (t->next) t = t->next;
                    tail = t;
                }
            }
            free(p->name);
            free(p->note);
            free(p);
            return;
        }
        curr = &(*curr)->next;
    }
}

static void rejoin_person(const char *name, const char *note) {
    // if exists, remove first? Spec: "REJOIN name note adds a person again" - likely append.
    // We'll remove any existing and append.
    Person *p = find_person(name);
    if (p) cancel_person(name);
    join_person(name, note);
}

static void find_person_note(const char *name) {
    Person *p = find_person(name);
    if (p) {
        printf("%s\n", p->note);
    }
}

static void queue_print(void) {
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "JOIN") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            join_person(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve_person();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = strtok(NULL, " ");
            if (name) cancel_person(name);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            rejoin_person(name, note);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_person_note(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue_print();
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