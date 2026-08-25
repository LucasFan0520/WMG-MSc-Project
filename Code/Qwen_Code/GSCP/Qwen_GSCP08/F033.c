// F033.c
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

Employee *head = NULL;

Employee *find_employee(char *id) {
    for (Employee *e = head; e; e = e->next) {
        if (strcmp(e->id, id) == 0) return e;
    }
    return NULL;
}

void add_employee(char *id, char *name, char *dept, char *title) {
    if (find_employee(id)) return;
    Employee *e = malloc(sizeof(Employee));
    e->id = strdup(id);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
    e->next = head;
    head = e;
}

void update_employee(char *id, char *name, char *dept, char *title) {
    Employee *e = find_employee(id);
    if (!e) return;
    free(e->name);
    free(e->department);
    free(e->title);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
}

void delete_employee(char *id) {
    Employee *prev = NULL;
    Employee *curr = head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->id);
            free(curr->name);
            free(curr->department);
            free(curr->title);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_employee_info(char *id) {
    Employee *e = find_employee(id);
    if (e) printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

void list_employees() {
    for (Employee *e = head; e; e = e->next) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
    }
}

int main() {
    char cmd[20], a[100], b[100], c[100], d[100];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s%99s%99s%99s", a, b, c, d);
            add_employee(a, b, c, d);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            scanf("%99s%99s%99s%99s", a, b, c, d);
            update_employee(a, b, c, d);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_employee(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", a);
            find_employee_info(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    return 0;
}