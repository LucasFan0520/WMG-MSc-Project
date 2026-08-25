// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct employee {
    char *id;
    char *name;
    char *department;
    char *title;
    struct employee *next;
};

struct employee *employees = NULL;

struct employee *find_employee(char *id) {
    struct employee *e = employees;
    while (e) {
        if (strcmp(e->id, id) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void add_employee(char *id, char *name, char *dept, char *title) {
    struct employee *e = find_employee(id);
    if (e) {
        free(e->name);
        free(e->department);
        free(e->title);
        e->name = malloc(strlen(name) + 1);
        strcpy(e->name, name);
        e->department = malloc(strlen(dept) + 1);
        strcpy(e->department, dept);
        e->title = malloc(strlen(title) + 1);
        strcpy(e->title, title);
    } else {
        e = malloc(sizeof(struct employee));
        e->id = malloc(strlen(id) + 1);
        strcpy(e->id, id);
        e->name = malloc(strlen(name) + 1);
        strcpy(e->name, name);
        e->department = malloc(strlen(dept) + 1);
        strcpy(e->department, dept);
        e->title = malloc(strlen(title) + 1);
        strcpy(e->title, title);
        e->next = employees;
        employees = e;
    }
}

void update_employee(char *id, char *name, char *dept, char *title) {
    struct employee *e = find_employee(id);
    if (e) {
        free(e->name);
        free(e->department);
        free(e->title);
        e->name = malloc(strlen(name) + 1);
        strcpy(e->name, name);
        e->department = malloc(strlen(dept) + 1);
        strcpy(e->department, dept);
        e->title = malloc(strlen(title) + 1);
        strcpy(e->title, title);
    }
}

void delete_employee(char *id) {
    struct employee *e = employees, *prev = NULL;
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

void find_employee_print(char *id) {
    struct employee *e = find_employee(id);
    if (e) printf("%s %s %s\n", e->name, e->department, e->title);
}

void list_employees() {
    struct employee *e = employees;
    while (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
        e = e->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256], name[256], dept[256], title[4096];
        int n = sscanf(line, "%31s %255s %255s %255s %4095[^\n]", cmd, id, name, dept, title);
        if (strcmp(cmd, "ADD") == 0 && n >= 4) {
            add_employee(id, name, dept, title);
        } else if (strcmp(cmd, "UPDATE") == 0 && n >= 4) {
            update_employee(id, name, dept, title);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_employee(id);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_employee_print(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    return 0;
}