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
int num_employees = 0;

int find_employee(const char *id) {
    for (int i = 0; i < num_employees; i++) {
        if (strcmp(employees[i].id, id) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "ADD") == 0) {
            char id[256], name[256], dept[256], title[256];
            sscanf(line, "%*s %s %s %s %s", id, name, dept, title);
            employees = realloc(employees, (num_employees + 1) * sizeof(Employee));
            employees[num_employees].id = strdup(id);
            employees[num_employees].name = strdup(name);
            employees[num_employees].department = strdup(dept);
            employees[num_employees].title = strdup(title);
            num_employees++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[256], name[256], dept[256], title[256];
            sscanf(line, "%*s %s %s %s %s", id, name, dept, title);
            int idx = find_employee(id);
            if (idx >= 0) {
                free(employees[idx].name);
                free(employees[idx].department);
                free(employees[idx].title);
                employees[idx].name = strdup(name);
                employees[idx].department = strdup(dept);
                employees[idx].title = strdup(title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            sscanf(line, "%*s %s", id);
            int idx = find_employee(id);
            if (idx >= 0) {
                free(employees[idx].id);
                free(employees[idx].name);
                free(employees[idx].department);
                free(employees[idx].title);
                for (int i = idx; i < num_employees - 1; i++) {
                    employees[i] = employees[i + 1];
                }
                num_employees--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char id[256];
            sscanf(line, "%*s %s", id);
            int idx = find_employee(id);
            if (idx >= 0) {
                printf("%s %s %s %s\n", employees[idx].id, employees[idx].name, employees[idx].department, employees[idx].title);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < num_employees; i++) {
                printf("%s %s %s %s\n", employees[i].id, employees[i].name, employees[i].department, employees[i].title);
            }
        }
    }
    
    for (int i = 0; i < num_employees; i++) {
        free(employees[i].id);
        free(employees[i].name);
        free(employees[i].department);
        free(employees[i].title);
    }
    free(employees);
    
    return 0;
}