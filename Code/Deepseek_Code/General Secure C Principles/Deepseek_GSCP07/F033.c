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
    Employee *e = head;
    while (e) {
        if (strcmp(e->id, id) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void free_employee(Employee *e) {
    free(e->id);
    free(e->name);
    free(e->department);
    free(e->title);
    free(e);
}

void handle_add(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    if (find_employee(id)) return;
    char *name = strtok(NULL, " ");
    if (!name) return;
    char *dept = strtok(NULL, " ");
    if (!dept) return;
    char *title = strtok(NULL, "");
    if (!title) title = "";
    Employee *new = malloc(sizeof(Employee));
    new->id = strdup(id);
    new->name = strdup(name);
    new->department = strdup(dept);
    new->title = strdup(title);
    new->next = NULL;
    if (!head) head = new;
    else {
        Employee *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_update(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    Employee *e = find_employee(id);
    if (!e) return;
    char *name = strtok(NULL, " ");
    if (!name) return;
    char *dept = strtok(NULL, " ");
    if (!dept) return;
    char *title = strtok(NULL, "");
    if (!title) title = "";
    free(e->name);
    free(e->department);
    free(e->title);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
}

void handle_delete(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    Employee *e = head, *prev = NULL;
    while (e) {
        if (strcmp(e->id, id) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            free_employee(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void handle_find(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    Employee *e = find_employee(id);
    if (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
    }
}

void handle_list() {
    Employee *e = head;
    while (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
        e = e->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) handle_add(cmd);
        else if (strcmp(cmd, "UPDATE") == 0) handle_update(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "FIND") == 0) handle_find(cmd);
        else if (strcmp(cmd, "LIST") == 0) handle_list();
    }
    while (head) {
        Employee *tmp = head;
        head = head->next;
        free_employee(tmp);
    }
    return 0;
}