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

Employee* find_employee(const char *id) {
    Employee *cur = head;
    while (cur) {
        if (strcmp(cur->id, id) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_employee(id)) return;
    Employee *new = malloc(sizeof(Employee));
    new->id = strdup(id);
    new->name = strdup(name);
    new->department = strdup(dept);
    new->title = strdup(title);
    new->next = NULL;
    if (!head) head = new;
    else {
        Employee *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
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
    Employee *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
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

void find_employee_print(const char *id) {
    Employee *e = find_employee(id);
    if (!e) return;
    printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

void list_employees() {
    Employee *cur = head;
    while (cur) {
        printf("%s %s %s %s\n", cur->id, cur->name, cur->department, cur->title);
        cur = cur->next;
    }
}

void free_all() {
    Employee *cur = head;
    while (cur) {
        Employee *tmp = cur;
        cur = cur->next;
        free(tmp->id);
        free(tmp->name);
        free(tmp->department);
        free(tmp->title);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *dept = strtok(NULL, " ");
            if (!dept) continue;
            char *title = strtok(NULL, "");
            if (!title) title = "";
            add_employee(id, name, dept, title);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *dept = strtok(NULL, " ");
            if (!dept) continue;
            char *title = strtok(NULL, "");
            if (!title) title = "";
            update_employee(id, name, dept, title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = strtok(NULL, " ");
            if (id) delete_employee(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = strtok(NULL, " ");
            if (id) find_employee_print(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    free_all();
    return 0;
}