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

Employee *head = NULL;

Employee *find_employee(char *id) {
    for (Employee *e = head; e; e = e->next)
        if (strcmp(e->id, id) == 0) return e;
    return NULL;
}

void add_employee(char *id, char *name, char *dept, char *title) {
    Employee *e = find_employee(id);
    if (e) {
        free(e->name);
        free(e->department);
        free(e->title);
        e->name = strdup(name);
        e->department = strdup(dept);
        e->title = strdup(title);
        return;
    }
    Employee *new = malloc(sizeof(Employee));
    new->id = strdup(id);
    new->name = strdup(name);
    new->department = strdup(dept);
    new->title = strdup(title);
    new->next = head;
    head = new;
}

void update_employee(char *id, char *name, char *dept, char *title) {
    add_employee(id, name, dept, title); // add handles update
}

void delete_employee(char *id) {
    Employee *prev = NULL;
    for (Employee *e = head; e; e = e->next) {
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
    }
}

void find_employee_print(char *id) {
    Employee *e = find_employee(id);
    if (e) printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

void list_all(void) {
    for (Employee *e = head; e; e = e->next)
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

void free_all(void) {
    Employee *e = head;
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[256], arg4[1024];
        int n = sscanf(line, "%15s %255s %255s %255s %[^\n]", cmd, arg1, arg2, arg3, arg4);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 5) {
            add_employee(arg1, arg2, arg3, arg4);
        } else if (strcmp(cmd, "UPDATE") == 0 && n == 5) {
            update_employee(arg1, arg2, arg3, arg4);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_employee(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_employee_print(arg1);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_all();
        }
    }
    free(line);
    free_all();
    return 0;
}