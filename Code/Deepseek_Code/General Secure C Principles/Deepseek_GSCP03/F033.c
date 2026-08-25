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
    if (!e) return;
    e->id = strdup(id);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
    e->next = head;
    head = e;
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
    Employee *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
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

void find_employee_cmd(const char *id) {
    Employee *e = find_employee(id);
    if (!e) {
        printf("NOT_FOUND\n");
        return;
    }
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
        Employee *tmp = e;
        e = e->next;
        free(tmp->id);
        free(tmp->name);
        free(tmp->department);
        free(tmp->title);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(line + 4, "%255s %255s %255s %255s", id, name, dept, title) == 4) {
                add_employee(id, name, dept, title);
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(line + 7, "%255s %255s %255s %255s", id, name, dept, title) == 4) {
                update_employee(id, name, dept, title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_employee(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_employee_cmd(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    free_all();
    return 0;
}