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

Employee *employees = NULL;

Employee *find_emp(const char *id) {
    for (Employee *e = employees; e; e = e->next) {
        if (strcmp(e->id, id) == 0) return e;
    }
    return NULL;
}

void add_emp(const char *id, const char *name, const char *dept, const char *title) {
    if (find_emp(id)) return;
    Employee *e = malloc(sizeof(Employee));
    e->id = strdup(id);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
    e->next = employees;
    employees = e;
}

void update_emp(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_emp(id);
    if (!e) return;
    free(e->name);
    free(e->department);
    free(e->title);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
}

void delete_emp(const char *id) {
    Employee **ep = &employees;
    while (*ep) {
        if (strcmp((*ep)->id, id) == 0) {
            Employee *tmp = *ep;
            *ep = tmp->next;
            free(tmp->id);
            free(tmp->name);
            free(tmp->department);
            free(tmp->title);
            free(tmp);
            return;
        }
        ep = &(*ep)->next;
    }
}

void find_emp_print(const char *id) {
    Employee *e = find_emp(id);
    if (e) printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

void list_emps() {
    for (Employee *e = employees; e; e = e->next) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
    }
}

void cleanup() {
    while (employees) {
        Employee *tmp = employees->next;
        free(employees->id);
        free(employees->name);
        free(employees->department);
        free(employees->title);
        free(employees);
        employees = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[256], a4[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %255s", a1, a2, a3, a4) == 4) add_emp(a1, a2, a3, a4);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %255s", a1, a2, a3, a4) == 4) update_emp(a1, a2, a3, a4);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_emp(a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) find_emp_print(a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_emps();
        }
    }
    cleanup();
    return 0;
}