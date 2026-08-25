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

Person *find_person(const char *name) {
    Person *p = front;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void join_queue(const char *name, const char *note) {
    if (find_person(name)) return;
    Person *p = malloc(sizeof(Person));
    if (!p) return;
    p->name = strdup(name);
    p->note = strdup(note);
    p->next = NULL;
    if (rear) rear->next = p;
    else front = p;
    rear = p;
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

void cancel(const char *name) {
    Person *prev = NULL, *curr = front;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else front = curr->next;
            if (curr == rear) rear = prev;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void rejoin(const char *name, const char *note) {
    // remove if exists, then add again
    cancel(name);
    join_queue(name, note);
}

void find(const char *name) {
    Person *p = find_person(name);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->note);
}

void queue_print(void) {
    Person *p = front;
    while (p) {
        printf("%s %s\n", p->name, p->note);
        p = p->next;
    }
}

void free_all(void) {
    Person *p = front;
    while (p) {
        Person *tmp = p;
        p = p->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "JOIN") == 0) {
            char name[256];
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    join_queue(name, note);
                }
            }
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            cancel(p);
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char name[256];
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    rejoin(name, note);
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find(p);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            queue_print();
        }
    }
    free_all();
    return 0;
}