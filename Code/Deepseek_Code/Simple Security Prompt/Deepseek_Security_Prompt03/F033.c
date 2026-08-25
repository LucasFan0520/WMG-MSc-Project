/*
 * F033.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct employee {
    char *id;
    char *name;
    char *department;
    char *title;
    struct employee *next;
} Employee;

Employee *employees = NULL;

Employee *find_employee(const char *id) {
    Employee *e = employees;
    while (e) {
        if (strcmp(e->id, id) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_employee(id)) return;
    Employee *e = malloc(sizeof(Employee));
    e->id = strdup(id);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
    e->next = employees;
    employees = e;
}

void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_employee(id);
    if (!e) return;
    free(e->name);
    free(e->department);
    free(e->title);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
}

void delete_employee(const char *id) {
    Employee *prev = NULL, *e = employees;
    while (e) {
        if (strcmp(e->id, id) == 0) {
            if (prev) prev->next = e->next;
            else employees = e->next;
            free(e->id);
            free(e->name);
            free(e->department);
            free(e->title);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void find_employee_cmd(const char *id) {
    Employee *e = find_employee(id);
    if (!e) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

void list_employees(void) {
    Employee *e = employees;
    while (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
        e = e->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], id[256], name[256], dept[256], title[256];
        if (sscanf(line, "%31s %255s %255s %255s %255s", cmd, id, name, dept, title) >= 4) {
            if (strcmp(cmd, "ADD") == 0) {
                add_employee(id, name, dept, title);
            } else if (strcmp(cmd, "UPDATE") == 0) {
                update_employee(id, name, dept, title);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_employee(id);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_employee_cmd(id);
            } else if (strcmp(cmd, "LIST") == 0) {
                list_employees();
            }
        }
    }
    return 0;
}