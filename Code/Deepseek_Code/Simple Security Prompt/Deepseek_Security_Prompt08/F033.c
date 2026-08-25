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

Employee *head = NULL, *tail = NULL;

Employee *find_employee(const char *id) {
    Employee *e = head;
    while (e) {
        if (strcmp(e->id, id) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_employee(id);
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
        return;
    }
    Employee *new = malloc(sizeof(Employee));
    new->id = malloc(strlen(id) + 1);
    strcpy(new->id, id);
    new->name = malloc(strlen(name) + 1);
    strcpy(new->name, name);
    new->department = malloc(strlen(dept) + 1);
    strcpy(new->department, dept);
    new->title = malloc(strlen(title) + 1);
    strcpy(new->title, title);
    new->next = NULL;
    if (tail) {
        tail->next = new;
        tail = new;
    } else {
        head = tail = new;
    }
}

void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_employee(id);
    if (!e) return;
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

void delete_employee(const char *id) {
    Employee *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
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
    if (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
    }
}

void list_employees(void) {
    Employee *e = head;
    while (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
        e = e->next;
    }
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *id = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            char *dept = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (id && name && dept && title) add_employee(id, name, dept, title);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *id = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            char *dept = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (id && name && dept && title) update_employee(id, name, dept, title);
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
    free(line);
    free_all();
    return 0;
}