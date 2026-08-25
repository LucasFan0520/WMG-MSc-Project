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
    if (!e) return;
    e->id = strdup(id);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
    e->next = NULL;
    if (!e->id || !e->name || !e->department || !e->title) {
        free(e->id); free(e->name); free(e->department); free(e->title); free(e);
        return;
    }
    if (!head) head = e;
    else {
        Employee *last = head;
        while (last->next) last = last->next;
        last->next = e;
    }
}

void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = find_employee(id);
    if (!e) return;
    char *nname = strdup(name);
    char *ndept = strdup(dept);
    char *ntitle = strdup(title);
    if (!nname || !ndept || !ntitle) {
        free(nname); free(ndept); free(ntitle);
        return;
    }
    free(e->name);
    free(e->department);
    free(e->title);
    e->name = nname;
    e->department = ndept;
    e->title = ntitle;
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
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], id[256], name[256], dept[256], title[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %4095[^\n]", id, name, dept, title) == 4)
                add_employee(id, name, dept, title);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %4095[^\n]", id, name, dept, title) == 4)
                update_employee(id, name, dept, title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                delete_employee(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                find_employee_print(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_employees();
        }
    }
    free(line);
    free_all();
    return 0;
}