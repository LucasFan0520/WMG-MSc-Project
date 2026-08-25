// filename: F033.c
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

static Employee *employees = NULL;

static void cleanup(void) {
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

static Employee *find_employee(const char *id) {
    for (Employee *e = employees; e; e = e->next)
        if (strcmp(e->id, id) == 0)
            return e;
    return NULL;
}

static void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_employee(id)) return;
    Employee *e = malloc(sizeof(Employee));
    if (!e) return;
    e->id = strdup(id);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
    if (!e->id || !e->name || !e->department || !e->title) {
        free(e->id);
        free(e->name);
        free(e->department);
        free(e->title);
        free(e);
        return;
    }
    e->next = employees;
    employees = e;
}

static void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_employee(id);
    if (!e) return;
    char *new_name = strdup(name);
    char *new_dept = strdup(dept);
    char *new_title = strdup(title);
    if (!new_name || !new_dept || !new_title) {
        free(new_name);
        free(new_dept);
        free(new_title);
        return;
    }
    free(e->name);
    free(e->department);
    free(e->title);
    e->name = new_name;
    e->department = new_dept;
    e->title = new_title;
}

static void delete_employee(const char *id) {
    Employee *prev = NULL;
    for (Employee *e = employees; e; e = e->next) {
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
    }
}

static void find_employee_print(const char *id) {
    Employee *e = find_employee(id);
    if (e) printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

static void list_employees(void) {
    for (Employee *e = employees; e; e = e->next)
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg4 = p;
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1 && *arg2) add_employee(arg1, arg2, arg3, arg4);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (*arg1 && *arg2) update_employee(arg1, arg2, arg3, arg4);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_employee(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_employee_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    free(line);
    return 0;
}