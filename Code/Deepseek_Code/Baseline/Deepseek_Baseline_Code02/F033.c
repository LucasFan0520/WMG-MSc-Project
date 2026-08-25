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
    e->id = malloc(strlen(id) + 1);
    strcpy(e->id, id);
    e->name = malloc(strlen(name) + 1);
    strcpy(e->name, name);
    e->department = malloc(strlen(dept) + 1);
    strcpy(e->department, dept);
    e->title = malloc(strlen(title) + 1);
    strcpy(e->title, title);
    e->next = head;
    head = e;
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *dept = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *title = p;
            add_employee(id, name, dept, title);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *dept = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *title = p;
            update_employee(id, name, dept, title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_employee(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_employee_print(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    free_all();
    return 0;
}