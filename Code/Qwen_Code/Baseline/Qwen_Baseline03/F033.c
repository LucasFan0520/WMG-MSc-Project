// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char id[100];
    char name[100];
    char dept[100];
    char title[100];
} Emp;

Emp emps[1000];
int en = 0;

int findEmp(char *id) {
    for (int i = 0; i < en; i++) {
        if (!strcmp(emps[i].id, id)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], id[100], name[100], dept[100], title[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s %s %s", id, name, dept, title);
            strcpy(emps[en].id, id);
            strcpy(emps[en].name, name);
            strcpy(emps[en].dept, dept);
            strcpy(emps[en].title, title);
            en++;
        } else if (!strcmp(cmd, "UPDATE")) {
            scanf("%s %s %s %s", id, name, dept, title);
            int i = findEmp(id);
            if (i != -1) {
                strcpy(emps[i].name, name);
                strcpy(emps[i].dept, dept);
                strcpy(emps[i].title, title);
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", id);
            int i = findEmp(id);
            if (i != -1) {
                for (int j = i; j < en - 1; j++) {
                    emps[j] = emps[j + 1];
                }
                en--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", id);
            int i = findEmp(id);
            if (i != -1) {
                printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < en; i++) {
                printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
            }
        }
    }
    return 0;
}