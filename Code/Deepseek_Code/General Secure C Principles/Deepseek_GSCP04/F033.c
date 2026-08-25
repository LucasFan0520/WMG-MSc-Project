/* F033.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Employee {
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

static Employee *employees = NULL;
static size_t ecount = 0, ecap = 0;

static Employee *find_employee(const char *id) {
    for (size_t i = 0; i < ecount; i++) {
        if (strcmp(employees[i].id, id) == 0)
            return &employees[i];
    }
    return NULL;
}

static void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (ecount == ecap) {
        ecap = ecap ? ecap * 2 : 4;
        employees = realloc(employees, ecap * sizeof(Employee));
        if (!employees) { perror("realloc"); exit(1); }
    }
    employees[ecount].id = strdup(id);
    employees[ecount].name = strdup(name);
    employees[ecount].dept = strdup(dept);
    employees[ecount].title = strdup(title);
    ecount++;
}

static void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_employee(id);
    if (!e) return;
    free(e->name); e->name = strdup(name);
    free(e->dept); e->dept = strdup(dept);
    free(e->title); e->title = strdup(title);
}

static void delete_employee(const char *id) {
    for (size_t i = 0; i < ecount; i++) {
        if (strcmp(employees[i].id, id) == 0) {
            free(employees[i].id);
            free(employees[i].name);
            free(employees[i].dept);
            free(employees[i].title);
            for (size_t j = i; j < ecount - 1; j++)
                employees[j] = employees[j + 1];
            ecount--;
            return;
        }
    }
}

static void find_employee_print(const char *id) {
    Employee *e = find_employee(id);
    if (!e) return;
    printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
}

static void list_employees(void) {
    for (size_t i = 0; i < ecount; i++) {
        printf("%s %s %s %s\n", employees[i].id, employees[i].name,
               employees[i].dept, employees[i].title);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
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
            if (find_employee(id)) {
                update_employee(id, name, dept, title);
            } else {
                add_employee(id, name, dept, title);
            }
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
    for (size_t i = 0; i < ecount; i++) {
        free(employees[i].id);
        free(employees[i].name);
        free(employees[i].dept);
        free(employees[i].title);
    }
    free(employees);
    return 0;
}