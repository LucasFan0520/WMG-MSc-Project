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

Employee *find_employee(const char *id) {
    Employee *curr = employees;
    while (curr) {
        if (strcmp(curr->id, id) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_employee(id)) return;
    Employee *e = malloc(sizeof(Employee));
    if (!e) exit(EXIT_FAILURE);
    e->id = strdup(id);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
    e->next = employees;
    employees = e;
}

void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_employee(id);
    if (!e) {
        add_employee(id, name, dept, title);
        return;
    }
    free(e->name);
    free(e->department);
    free(e->title);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
}

void delete_employee(const char *id) {
    Employee *curr = employees;
    Employee *prev = NULL;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else employees = curr->next;
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

void find_employee_print(const char *id) {
    Employee *e = find_employee(id);
    if (!e) return;
    printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

void list_employees() {
    Employee *curr = employees;
    while (curr) {
        printf("%s %s %s %s\n", curr->id, curr->name, curr->department, curr->title);
        curr = curr->next;
    }
}

void free_all() {
    Employee *curr = employees;
    while (curr) {
        Employee *next = curr->next;
        free(curr->id);
        free(curr->name);
        free(curr->department);
        free(curr->title);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[256] = {0};
        char arg4[3200] = {0};
        
        int matched = sscanf(line, "%31s %255s %255s %255s %[^\n]", cmd, arg1, arg2, arg3, arg4);
        
        if (strcmp(cmd, "ADD") == 0 && matched >= 4) {
            add_employee(arg1, arg2, arg3, arg4);
        } else if (strcmp(cmd, "UPDATE") == 0 && matched >= 4) {
            update_employee(arg1, arg2, arg3, arg4);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_employee(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_employee_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    free_all();
    return 0;
}