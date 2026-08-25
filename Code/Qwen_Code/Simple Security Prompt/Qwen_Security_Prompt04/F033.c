// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *dept;
    char *title;
} Emp;

Emp emps[1000];
int ec = 0;

int find_id(char *id) {
    for (int i = 0; i < ec; i++) if (strcmp(emps[i].id, id) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char id[1000], name[1000], dept[1000], title[1000];
            sscanf(rest, "%s %s %s %s", id, name, dept, title);
            if (find_id(id) == -1) {
                emps[ec].id = strdup(id);
                emps[ec].name = strdup(name);
                emps[ec].dept = strdup(dept);
                emps[ec].title = strdup(title);
                ec++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[1000], name[1000], dept[1000], title[1000];
            sscanf(rest, "%s %s %s %s", id, name, dept, title);
            int idx = find_id(id);
            if (idx != -1) {
                free(emps[idx].name);
                free(emps[idx].dept);
                free(emps[idx].title);
                emps[idx].name = strdup(name);
                emps[idx].dept = strdup(dept);
                emps[idx].title = strdup(title);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = find_id(rest);
            if (idx != -1) {
                free(emps[idx].id);
                free(emps[idx].name);
                free(emps[idx].dept);
                free(emps[idx].title);
                for (int i = idx; i < ec - 1; i++) emps[i] = emps[i+1];
                ec--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            int idx = find_id(rest);
            if (idx != -1) {
                printf("%s %s %s %s\n", emps[idx].id, emps[idx].name, emps[idx].dept, emps[idx].title);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < ec; i++) {
                printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
            }
        }
    }
    for (int i = 0; i < ec; i++) {
        free(emps[i].id);
        free(emps[i].name);
        free(emps[i].dept);
        free(emps[i].title);
    }
    return 0;
}