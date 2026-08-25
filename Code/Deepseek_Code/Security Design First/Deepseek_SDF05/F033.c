// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_employee(id)) return;
    Employee *e = malloc(sizeof(Employee));
    if (!e) return;
    e->id = safe_strdup(id);
    e->name = safe_strdup(name);
    e->department = safe_strdup(dept);
    e->title = safe_strdup(title);
    e->next = employees;
    employees = e;
}

void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_employee(id);
    if (!e) return;
    free(e->name);
    free(e->department);
    free(e->title);
    e->name = safe_strdup(name);
    e->department = safe_strdup(dept);
    e->title = safe_strdup(title);
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

void free_all(void) {
    Employee *e = employees;
    while (e) {
        Employee *next = e->next;
        free(e->id);
        free(e->name);
        free(e->department);
        free(e->title);
        free(e);
        e = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512], arg3[512], arg4[512];
        int n = sscanf(line, "%31s %511s %511s %511s %511s", cmd, arg1, arg2, arg3, arg4);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 5) {
            add_employee(arg1, arg2, arg3, arg4);
        } else if (strcmp(cmd, "UPDATE") == 0 && n >= 5) {
            update_employee(arg1, arg2, arg3, arg4);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_employee(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_employee_cmd(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    free_all();
    return 0;
}