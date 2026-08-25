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

Employee *find_employee(const char *id) {
    Employee *e = head;
    while (e) {
        if (strcmp(e->id, id) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_employee(id)) return;
    Employee *e = malloc(sizeof(Employee));
    if (!e) return;
    e->id = strdup(id);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
    e->next = head;
    head = e;
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
    Employee *prev = NULL;
    Employee *e = head;
    while (e) {
        if (strcmp(e->id, id) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
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

void find_employee_print(const char *id) {
    Employee *e = find_employee(id);
    if (!e) return;
    printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

void list_employees(void) {
    Employee *e = head;
    while (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
        e = e->next;
    }
}

void free_all(void) {
    while (head) {
        Employee *next = head->next;
        free(head->id);
        free(head->name);
        free(head->department);
        free(head->title);
        free(head);
        head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(line, "%*s %255s %255s %255s %255s", id, name, dept, title) == 4) {
                add_employee(id, name, dept, title);
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(line, "%*s %255s %255s %255s %255s", id, name, dept, title) == 4) {
                update_employee(id, name, dept, title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) {
                delete_employee(id);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) {
                find_employee_print(id);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }

    free(line);
    free_all();
    return 0;
}