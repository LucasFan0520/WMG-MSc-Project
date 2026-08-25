// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; char *name; char *dept; char *title; } Emp;
Emp *emps = NULL;
int ne = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], id[256], name[256], dept[256], title[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            sscanf(line, "%*s %s %s %s %s", id, name, dept, title);
            int idx = -1;
            for (int i = 0; i < ne; i++) if (strcmp(emps[i].id, id) == 0) { idx = i; break; }
            if (idx != -1) {
                free(emps[idx].name); free(emps[idx].dept); free(emps[idx].title);
                emps[idx].name = strdup(name);
                emps[idx].dept = strdup(dept);
                emps[idx].title = strdup(title);
            } else {
                emps = realloc(emps, (ne + 1) * sizeof(Emp));
                emps[ne].id = strdup(id);
                emps[ne].name = strdup(name);
                emps[ne].dept = strdup(dept);
                emps[ne].title = strdup(title);
                ne++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", id);
            for (int i = 0; i < ne; i++) {
                if (strcmp(emps[i].id, id) == 0) {
                    free(emps[i].id); free(emps[i].name); free(emps[i].dept); free(emps[i].title);
                    for (int j = i; j < ne - 1; j++) emps[j] = emps[j + 1];
                    ne--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", id);
            for (int i = 0; i < ne; i++) {
                if (strcmp(emps[i].id, id) == 0) {
                    printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < ne; i++) {
                printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
            }
        }
    }
    for (int i = 0; i < ne; i++) {
        free(emps[i].id); free(emps[i].name); free(emps[i].dept); free(emps[i].title);
    }
    free(emps);
    return 0;
}