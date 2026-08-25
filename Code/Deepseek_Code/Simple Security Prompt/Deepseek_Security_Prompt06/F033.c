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
    Employee *e = head, *prev = NULL;
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
    if (e) printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
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
        Employee *tmp = e->next;
        free(e->id);
        free(e->name);
        free(e->department);
        free(e->title);
        free(e);
        e = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256], name[256], dept[256], title[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s %255s %255s", id, name, dept) == 3) {
                const char *t = rest + strlen(id) + 1 + strlen(name) + 1 + strlen(dept) + 1;
                while (*t == ' ') t++;
                if (*t) add_employee(id, name, dept, t);
                else add_employee(id, name, dept, "");
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(rest, "%255s %255s %255s", id, name, dept) == 3) {
                const char *t = rest + strlen(id) + 1 + strlen(name) + 1 + strlen(dept) + 1;
                while (*t == ' ') t++;
                if (*t) update_employee(id, name, dept, t);
                else update_employee(id, name, dept, "");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", id) == 1) delete_employee(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", id) == 1) find_employee_print(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    free_all();
    return 0;
}