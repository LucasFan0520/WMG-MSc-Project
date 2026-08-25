// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; char *name; char *dept; char *title; } Employee;
Employee *emps = NULL;
int ec = 0, ecap = 0;

int find_emp(const char *id) {
    for (int i = 0; i < ec; i++)
        if (strcmp(emps[i].id, id) == 0) return i;
    return -1;
}

void add_employee(const char *id, const char *name, const char *dept, const char *title) {
    if (find_emp(id) >= 0) return;
    if (ec == ecap) { ecap = ecap ? ecap * 2 : 4; emps = realloc(emps, sizeof(Employee) * ecap); }
    emps[ec].id = strdup(id);
    emps[ec].name = strdup(name);
    emps[ec].dept = strdup(dept);
    emps[ec].title = strdup(title);
    ec++;
}

void update_employee(const char *id, const char *name, const char *dept, const char *title) {
    int idx = find_emp(id);
    if (idx < 0) return;
    free(emps[idx].name);
    free(emps[idx].dept);
    free(emps[idx].title);
    emps[idx].name = strdup(name);
    emps[idx].dept = strdup(dept);
    emps[idx].title = strdup(title);
}

void delete_employee(const char *id) {
    int idx = find_emp(id);
    if (idx < 0) return;
    free(emps[idx].id);
    free(emps[idx].name);
    free(emps[idx].dept);
    free(emps[idx].title);
    for (int i = idx; i < ec - 1; i++) emps[i] = emps[i + 1];
    ec--;
}

void find_employee(const char *id) {
    int idx = find_emp(id);
    if (idx < 0) return;
    printf("%s %s %s %s\n", emps[idx].id, emps[idx].name, emps[idx].dept, emps[idx].title);
}

void list_employees(void) {
    for (int i = 0; i < ec; i++)
        printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], id[256], name[256], dept[256], title[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s %4095[^\n]", id, name, dept, title) == 4) {
                if (strcmp(cmd, "ADD") == 0) add_employee(id, name, dept, title);
                else update_employee(id, name, dept, title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) delete_employee(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) find_employee(id);
        } else if (strcmp(cmd, "LIST") == 0) list_employees();
    }
    for (int i = 0; i < ec; i++) {
        free(emps[i].id);
        free(emps[i].name);
        free(emps[i].dept);
        free(emps[i].title);
    }
    free(emps);
    return 0;
}