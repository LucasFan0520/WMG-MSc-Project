// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Employee {
    char *id;
    char *name;
    char *department;
    char *title;
    struct Employee *next;
} Employee;

Employee *employees = NULL;

Employee* find_employee(const char *id) {
    for (Employee *e = employees; e; e = e->next)
        if (strcmp(e->id, id) == 0) return e;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(rest, "%255s %255s %255s %255s", id, name, dept, title) != 4) continue;
            Employee *e = find_employee(id);
            if (e) {
                free(e->name); free(e->department); free(e->title);
                e->name = strdup(name);
                e->department = strdup(dept);
                e->title = strdup(title);
            } else {
                e = malloc(sizeof(Employee));
                if (!e) continue;
                e->id = strdup(id);
                e->name = strdup(name);
                e->department = strdup(dept);
                e->title = strdup(title);
                if (!e->id || !e->name || !e->department || !e->title) {
                    free(e->id); free(e->name); free(e->department); free(e->title); free(e); continue;
                }
                e->next = employees;
                employees = e;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) != 1) continue;
            Employee *prev = NULL;
            for (Employee *e = employees; e; prev = e, e = e->next) {
                if (strcmp(e->id, id) == 0) {
                    if (prev) prev->next = e->next;
                    else employees = e->next;
                    free(e->id); free(e->name); free(e->department); free(e->title);
                    free(e);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char id[256];
            if (sscanf(rest, "%255s", id) != 1) continue;
            Employee *e = find_employee(id);
            if (e) printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Employee *e = employees; e; e = e->next) {
                printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
            }
        }
    }
    while (employees) {
        Employee *e = employees;
        employees = employees->next;
        free(e->id); free(e->name); free(e->department); free(e->title);
        free(e);
    }
    return 0;
}