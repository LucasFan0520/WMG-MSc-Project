// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *dept;
    char *title;
} Employee;

Employee *emps = NULL;
int ecount = 0;

int find_emp(const char *id) {
    for (int i = 0; i < ecount; i++) {
        if (strcmp(emps[i].id, id) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char id[100], name[100], dept[100], title[100];
            scanf("%s %s %s %s", id, name, dept, title);
            if (find_emp(id) == -1) {
                emps = realloc(emps, sizeof(Employee) * (ecount + 1));
                emps[ecount].id = strdup(id);
                emps[ecount].name = strdup(name);
                emps[ecount].dept = strdup(dept);
                emps[ecount].title = strdup(title);
                ecount++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[100], name[100], dept[100], title[100];
            scanf("%s %s %s %s", id, name, dept, title);
            int idx = find_emp(id);
            if (idx != -1) {
                free(emps[idx].name);
                free(emps[idx].dept);
                free(emps[idx].title);
                emps[idx].name = strdup(name);
                emps[idx].dept = strdup(dept);
                emps[idx].title = strdup(title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[100];
            scanf("%s", id);
            int idx = find_emp(id);
            if (idx != -1) {
                free(emps[idx].id);
                free(emps[idx].name);
                free(emps[idx].dept);
                free(emps[idx].title);
                for (int i = idx; i < ecount - 1; i++) emps[i] = emps[i+1];
                ecount--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char id[100];
            scanf("%s", id);
            int idx = find_emp(id);
            if (idx != -1) {
                printf("%s %s %s %s\n", emps[idx].id, emps[idx].name, emps[idx].dept, emps[idx].title);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < ecount; i++) {
                printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
            }
        }
    }
    for (int i = 0; i < ecount; i++) {
        free(emps[i].id);
        free(emps[i].name);
        free(emps[i].dept);
        free(emps[i].title);
    }
    free(emps);
    return 0;
}