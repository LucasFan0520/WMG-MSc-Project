/* F033.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Employee {
    char *id;
    char *name;
    char *department;
    char *title;
    struct Employee *next;
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
    if (e) {
        free(e->name);
        free(e->department);
        free(e->title);
        e->name = strdup(name);
        e->department = strdup(dept);
        e->title = strdup(title);
    }
}

void delete_employee(const char *id) {
    Employee *prev = NULL, *cur = employees;
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            if (prev) prev->next = cur->next;
            else employees = cur->next;
            free(cur->id);
            free(cur->name);
            free(cur->department);
            free(cur->title);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_employee(Employee *e) {
    printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

void list_employees(void) {
    Employee *e = employees;
    while (e) {
        printf("%s\n", e->id);
        e = e->next;
    }
}

void free_employees(void) {
    while (employees) {
        Employee *tmp = employees;
        employees = employees->next;
        free(tmp->id);
        free(tmp->name);
        free(tmp->department);
        free(tmp->title);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char id[256], name[256], dept[256], title[256];
        int n = sscanf(line, "%s %s %s %s %s", cmd, id, name, dept, title);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 5) continue;
            add_employee(id, name, dept, title);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (n < 5) continue;
            update_employee(id, name, dept, title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_employee(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            Employee *e = find_employee(id);
            if (e) print_employee(e);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    free_employees();
    return 0;
}