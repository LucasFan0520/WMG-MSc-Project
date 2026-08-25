// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Employee { char *id; char *name; char *dept; char *title; struct Employee *next; } Employee;
Employee *employees = NULL;

Employee *find_employee(const char *id) {
    for (Employee *e = employees; e; e = e->next)
        if (strcmp(e->id, id) == 0) return e;
    return NULL;
}

void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_employee(id)) return;
    Employee *e = malloc(sizeof(Employee));
    if (!e) return;
    e->id = strdup(id);
    e->name = strdup(name);
    e->dept = strdup(dept);
    e->title = strdup(title);
    e->next = employees;
    employees = e;
}

void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_employee(id);
    if (!e) return;
    free(e->name); free(e->dept); free(e->title);
    e->name = strdup(name);
    e->dept = strdup(dept);
    e->title = strdup(title);
}

void delete_employee(const char *id) {
    Employee *prev = NULL;
    for (Employee *e = employees; e; prev = e, e = e->next) {
        if (strcmp(e->id, id) == 0) {
            if (prev) prev->next = e->next; else employees = e->next;
            free(e->id); free(e->name); free(e->dept); free(e->title); free(e);
            return;
        }
    }
}

void find_employee_print(const char *id) {
    Employee *e = find_employee(id);
    if (!e) { printf("NOT_FOUND\n"); return; }
    printf("%s %s %s\n", e->name, e->dept, e->title);
}

void list_employees(void) {
    for (Employee *e = employees; e; e = e->next)
        printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char id[256], name[256], dept[256], title[4096];
            if (sscanf(rest, "%255s %255s %255s", id, name, dept) == 3) {
                char *t = rest + strlen(id) + 1 + strlen(name) + 1 + strlen(dept);
                while (*t == ' ') t++;
                strcpy(title, t);
                add_employee(id, name, dept, title);
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[256], name[256], dept[256], title[4096];
            if (sscanf(rest, "%255s %255s %255s", id, name, dept) == 3) {
                char *t = rest + strlen(id) + 1 + strlen(name) + 1 + strlen(dept);
                while (*t == ' ') t++;
                strcpy(title, t);
                update_employee(id, name, dept, title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) == 1) delete_employee(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) == 1) find_employee_print(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    while (employees) { Employee *e = employees; employees = e->next; free(e->id); free(e->name); free(e->dept); free(e->title); free(e); }
    return 0;
}