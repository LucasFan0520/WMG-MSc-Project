// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Emp {
    char *id;
    char *name;
    char *dept;
    char *title;
    struct Emp *next;
} Emp;

Emp *head = NULL;

void add_emp(const char *id, const char *name, const char *dept, const char *title) {
    Emp *e = malloc(sizeof(Emp));
    e->id = strdup(id);
    e->name = strdup(name);
    e->dept = strdup(dept);
    e->title = strdup(title);
    e->next = head;
    head = e;
}

void update_emp(const char *id, const char *name, const char *dept, const char *title) {
    for (Emp *e = head; e; e = e->next) {
        if (strcmp(e->id, id) == 0) {
            free(e->name); free(e->dept); free(e->title);
            e->name = strdup(name);
            e->dept = strdup(dept);
            e->title = strdup(title);
            return;
        }
    }
}

void delete_emp(const char *id) {
    Emp *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->id); free(p->name); free(p->dept); free(p->title); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void find_emp(const char *id) {
    for (Emp *e = head; e; e = e->next) {
        if (strcmp(e->id, id) == 0) {
            printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
            return;
        }
    }
}

void list_emp() {
    for (Emp *e = head; e; e = e->next) {
        printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[1000], c[1000], d[1000];
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %s %s %s", a, b, c, d);
            add_emp(a, b, c, d);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            sscanf(line, "%*s %s %s %s %s", a, b, c, d);
            update_emp(a, b, c, d);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_emp(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            find_emp(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_emp();
        }
    }
    while (head) {
        Emp *n = head->next;
        free(head->id); free(head->name); free(head->dept); free(head->title); free(head);
        head = n;
    }
    return 0;
}