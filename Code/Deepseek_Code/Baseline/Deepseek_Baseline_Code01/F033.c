// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct emp {
    char *id;
    char *name;
    char *dept;
    char *title;
    struct emp *next;
} Emp;

Emp *head = NULL;
Emp *tail = NULL;

Emp *find_emp(const char *id) {
    Emp *e = head;
    while (e) {
        if (strcmp(e->id, id) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void add_emp(const char *id, const char *name, const char *dept, const char *title) {
    if (find_emp(id)) return;
    Emp *e = malloc(sizeof(Emp));
    e->id = strdup(id);
    e->name = strdup(name);
    e->dept = strdup(dept);
    e->title = strdup(title);
    e->next = NULL;
    if (tail) tail->next = e;
    else head = e;
    tail = e;
}

void update_emp(const char *id, const char *name, const char *dept, const char *title) {
    Emp *e = find_emp(id);
    if (!e) return;
    free(e->name);
    free(e->dept);
    free(e->title);
    e->name = strdup(name);
    e->dept = strdup(dept);
    e->title = strdup(title);
}

void delete_emp(const char *id) {
    Emp *e = head, *prev = NULL;
    while (e) {
        if (strcmp(e->id, id) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            if (e == tail) tail = prev;
            free(e->id);
            free(e->name);
            free(e->dept);
            free(e->title);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void find_emp_print(const char *id) {
    Emp *e = find_emp(id);
    if (e) printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
}

void list_emps(void) {
    Emp *e = head;
    while (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
        e = e->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256], name[256], dept[256], title[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %[^\n]", id, name, dept, title) == 4)
                add_emp(id, name, dept, title);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %[^\n]", id, name, dept, title) == 4)
                update_emp(id, name, dept, title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                delete_emp(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                find_emp_print(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_emps();
        }
    }
    return 0;
}