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

void add_emp(const char *id, const char *name, const char *dept, const char *title) {
    Employee *e = malloc(sizeof(Employee));
    if (!e) return;
    e->id = strdup(id);
    e->name = strdup(name);
    e->department = strdup(dept);
    e->title = strdup(title);
    if (!e->id || !e->name || !e->department || !e->title) {
        free(e->id); free(e->name); free(e->department); free(e->title); free(e); return;
    }
    e->next = head;
    head = e;
}

void update_emp(const char *id, const char *name, const char *dept, const char *title) {
    for (Employee *e = head; e; e = e->next) {
        if (strcmp(e->id, id) == 0) {
            free(e->name);
            free(e->department);
            free(e->title);
            e->name = strdup(name);
            e->department = strdup(dept);
            e->title = strdup(title);
            return;
        }
    }
}

void delete_emp(const char *id) {
    Employee *curr = head;
    Employee *prev = NULL;
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

void find_emp(const char *id) {
    for (Employee *e = head; e; e = e->next) {
        if (strcmp(e->id, id) == 0) {
            printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
            return;
        }
    }
}

void list_emps() {
    for (Employee *e = head; e; e = e->next) {
        printf("%s %s %s %s\n", e->id, e->name, e->department, e->title);
    }
}

void free_all() {
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
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[1024] = {0};
        char arg3[1024] = {0};
        char arg4[1024] = {0};
        
        int n = sscanf(line, "%63s %1023s %1023s %1023s %1023s", cmd, arg1, arg2, arg3, arg4);
        if (n < 1) continue;
        
        if (strcmp(cmd, "ADD") == 0 && n >= 5) {
            add_emp(arg1, arg2, arg3, arg4);
        } else if (strcmp(cmd, "UPDATE") == 0 && n >= 5) {
            update_emp(arg1, arg2, arg3, arg4);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_emp(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_emp(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_emps();
        }
    }
    free_all();
    return 0;
}