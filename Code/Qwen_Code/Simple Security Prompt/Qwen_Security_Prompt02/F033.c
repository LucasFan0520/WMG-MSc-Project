// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

Employee *employees = NULL;
int e_count = 0;

int find_id(const char *id) {
    for (int i = 0; i < e_count; i++) {
        if (strcmp(employees[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char id[1024];
    char name[1024];
    char dept[1024];
    char title[1024];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s %s %s", id, name, dept, title);
            employees = realloc(employees, sizeof(Employee) * (e_count + 1));
            employees[e_count].id = strdup(id);
            employees[e_count].name = strdup(name);
            employees[e_count].department = strdup(dept);
            employees[e_count].title = strdup(title);
            e_count++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            scanf("%s %s %s %s", id, name, dept, title);
            int idx = find_id(id);
            if (idx != -1) {
                free(employees[idx].name);
                free(employees[idx].department);
                free(employees[idx].title);
                employees[idx].name = strdup(name);
                employees[idx].department = strdup(dept);
                employees[idx].title = strdup(title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                free(employees[idx].id);
                free(employees[idx].name);
                free(employees[idx].department);
                free(employees[idx].title);
                for (int i = idx; i < e_count - 1; i++) {
                    employees[i] = employees[i + 1];
                }
                e_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", id);
            int idx = find_id(id);
            if (idx != -1) {
                printf("%s %s %s %s\n", employees[idx].id, employees[idx].name, employees[idx].department, employees[idx].title);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < e_count; i++) {
                printf("%s %s %s %s\n", employees[i].id, employees[i].name, employees[i].department, employees[i].title);
            }
        }
    }

    for (int i = 0; i < e_count; i++) {
        free(employees[i].id);
        free(employees[i].name);
        free(employees[i].department);
        free(employees[i].title);
    }
    free(employees);

    return 0;
}