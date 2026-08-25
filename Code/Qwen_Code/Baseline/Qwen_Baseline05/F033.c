// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ID 100
#define MAX_NAME 100
#define MAX_DEPT 100
#define MAX_TITLE 100

typedef struct {
    char id[MAX_ID];
    char name[MAX_NAME];
    char department[MAX_DEPT];
    char title[MAX_TITLE];
} Employee;

Employee employees[1000];
int emp_count = 0;

int find_emp(const char *id) {
    for (int i = 0; i < emp_count; i++) if (strcmp(employees[i].id, id) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], id[MAX_ID], name[MAX_NAME], dept[MAX_DEPT], title[MAX_TITLE];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s %s %s", id, name, dept, title);
            strcpy(employees[emp_count].id, id);
            strcpy(employees[emp_count].name, name);
            strcpy(employees[emp_count].department, dept);
            strcpy(employees[emp_count].title, title);
            emp_count++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            scanf("%s %s %s %s", id, name, dept, title);
            int idx = find_emp(id);
            if (idx != -1) {
                strcpy(employees[idx].name, name);
                strcpy(employees[idx].department, dept);
                strcpy(employees[idx].title, title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", id);
            int idx = find_emp(id);
            if (idx != -1) {
                for (int i = idx; i < emp_count - 1; i++) employees[i] = employees[i + 1];
                emp_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", id);
            int idx = find_emp(id);
            if (idx != -1) printf("%s %s %s %s\n", employees[idx].id, employees[idx].name, employees[idx].department, employees[idx].title);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < emp_count; i++) printf("%s %s %s %s\n", employees[i].id, employees[i].name, employees[i].department, employees[i].title);
        }
    }
    return 0;
}