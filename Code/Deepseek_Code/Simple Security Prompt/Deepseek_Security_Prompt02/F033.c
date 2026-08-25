/* F033.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Employee {
    char *id;
    char *name;
    char *department;
    char *title;
    struct Employee *next;
} Employee;

Employee *employees = NULL;

Employee *find_employee(const char *id) {
    Employee *e = employees;
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

void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_employee(id)) return;
    Employee *e = malloc(sizeof(Employee));
    e->id = strdup(id);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
    e->next = employees;
    employees = e;
}

void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_employee(id);
    if (!e) return;
    free(e->name); free(e->department); free(e->title);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
}

void delete_employee(const char *id) {
    Employee *prev = NULL, *curr = employees;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else employees = curr->next;
            free_employee(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_employee_print(const char *id) {
    Employee *e = find_employee(id);
    if (!e) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
}

void list_employees(void) {
    Employee *e = employees;
    while (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
        e = e->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char id[256], name[256], dept[256], title[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s %255s", id, name, dept) != 3) continue;
            p += strlen(id) + strlen(name) + strlen(dept) + 3;
            while (*p == ' ') p++;
            add_employee(id, name, dept, p);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s %255s", id, name, dept) != 3) continue;
            p += strlen(id) + strlen(name) + strlen(dept) + 3;
            while (*p == ' ') p++;
            update_employee(id, name, dept, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", id) == 1) delete_employee(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", id) == 1) find_employee_print(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }

    while (employees) {
        Employee *tmp = employees;
        employees = employees->next;
        free_employee(tmp);
    }
    return 0;
}