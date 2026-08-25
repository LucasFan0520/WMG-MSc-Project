// F033.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Employee {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

static Employee *employees = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_employee(const char *id) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(employees[i].id, id) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_employee(id) != -1) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Employee *new = realloc(employees, capacity * sizeof(Employee));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        employees = new;
    }
    employees[count].id = strdup(id);
    employees[count].name = strdup(name);
    employees[count].department = strdup(dept);
    employees[count].title = strdup(title);
    if (!employees[count].id || !employees[count].name ||
        !employees[count].department || !employees[count].title)
        { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    ssize_t idx = find_employee(id);
    if (idx == -1) return;
    free(employees[idx].name);
    free(employees[idx].department);
    free(employees[idx].title);
    employees[idx].name = strdup(name);
    employees[idx].department = strdup(dept);
    employees[idx].title = strdup(title);
    if (!employees[idx].name || !employees[idx].department || !employees[idx].title)
        { fprintf(stderr, "Memory error\n"); exit(1); }
}

static void delete_employee(const char *id) {
    ssize_t idx = find_employee(id);
    if (idx == -1) return;
    free(employees[idx].id);
    free(employees[idx].name);
    free(employees[idx].department);
    free(employees[idx].title);
    for (size_t i = idx; i < count - 1; i++)
        employees[i] = employees[i + 1];
    count--;
}

static void find_employee_print(const char *id) {
    ssize_t idx = find_employee(id);
    if (idx == -1) return;
    printf("%s %s %s %s\n", employees[idx].id, employees[idx].name,
           employees[idx].department, employees[idx].title);
}

static void list_employees(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s %s %s\n", employees[i].id, employees[i].name,
               employees[i].department, employees[i].title);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(employees[i].id);
        free(employees[i].name);
        free(employees[i].department);
        free(employees[i].title);
    }
    free(employees);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], id[256], name[256], dept[256], title[256];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %255s", id, name, dept, title) == 4)
                add_employee(id, name, dept, title);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %255s", id, name, dept, title) == 4)
                update_employee(id, name, dept, title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                delete_employee(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                find_employee_print(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }

    free(line);
    free_all();
    return 0;
}