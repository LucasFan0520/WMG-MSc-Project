// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMP 1000

typedef struct {
    char id[100];
    char name[100];
    char dept[100];
    char title[100];
} Emp;

Emp emps[MAX_EMP];
int ecount = 0;

int find_emp(const char *id) {
    for (int i = 0; i < ecount; i++) if (strcmp(emps[i].id, id) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], id[100], name[100], dept[100], title[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s %s %s", id, name, dept, title);
            if (find_emp(id) == -1 && ecount < MAX_EMP) {
                strcpy(emps[ecount].id, id);
                strcpy(emps[ecount].name, name);
                strcpy(emps[ecount].dept, dept);
                strcpy(emps[ecount].title, title);
                ecount++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            scanf("%s %s %s %s", id, name, dept, title);
            int idx = find_emp(id);
            if (idx != -1) {
                strcpy(emps[idx].name, name);
                strcpy(emps[idx].dept, dept);
                strcpy(emps[idx].title, title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", id);
            int idx = find_emp(id);
            if (idx != -1) {
                for (int i = idx; i < ecount - 1; i++) emps[i] = emps[i + 1];
                ecount--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
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
    return 0;
}