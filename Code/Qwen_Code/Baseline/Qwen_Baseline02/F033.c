// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; char *name; char *dept; char *title; } Emp;
Emp emps[10000]; int nemps = 0;

int find_emp(char *id) {
    for (int i = 0; i < nemps; i++) if (!strcmp(emps[i].id, id)) return i;
    return -1;
}

int main() {
    char cmd[20], id[100], name[100], dept[100], title[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s %s", id, name, dept);
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            emps[nemps].id = strdup(id);
            emps[nemps].name = strdup(name);
            emps[nemps].dept = strdup(dept);
            emps[nemps++].title = strdup(title);
        } else if (!strcmp(cmd, "UPDATE")) {
            scanf("%s %s %s", id, name, dept);
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            int idx = find_emp(id);
            if (idx != -1) {
                free(emps[idx].name); free(emps[idx].dept); free(emps[idx].title);
                emps[idx].name = strdup(name);
                emps[idx].dept = strdup(dept);
                emps[idx].title = strdup(title);
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", id);
            int idx = find_emp(id);
            if (idx != -1) {
                free(emps[idx].id); free(emps[idx].name); free(emps[idx].dept); free(emps[idx].title);
                for (int i = idx; i < nemps - 1; i++) emps[i] = emps[i + 1];
                nemps--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", id);
            int idx = find_emp(id);
            if (idx != -1) printf("%s %s %s %s\n", emps[idx].id, emps[idx].name, emps[idx].dept, emps[idx].title);
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < nemps; i++) {
                printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
            }
        }
    }
    for (int i = 0; i < nemps; i++) {
        free(emps[i].id); free(emps[i].name); free(emps[i].dept); free(emps[i].title);
    }
    return 0;
}