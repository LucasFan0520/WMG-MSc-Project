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

Employee *emps = NULL;
int ec = 0;

int find_emp(const char *id) {
    for (int i = 0; i < ec; i++) {
        if (strcmp(emps[i].id, id) == 0) return i;
    }
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[256];
        char a3[256];
        char a4[256];
        a1[0] = '\0';
        a2[0] = '\0';
        a3[0] = '\0';
        a4[0] = '\0';
        int n = sscanf(line, "%31s %255s %255s %255s %255[^\n]", cmd, a1, a2, a3, a4);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 5) {
            if (find_emp(a1) < 0) {
                emps = realloc(emps, (ec + 1) * sizeof(Employee));
                if (!emps) return 1;
                emps[ec].id = strdup(a1);
                emps[ec].name = strdup(a2);
                emps[ec].department = strdup(a3);
                emps[ec].title = strdup(a4);
                ec++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0 && n >= 5) {
            int ei = find_emp(a1);
            if (ei >= 0) {
                free(emps[ei].name);
                free(emps[ei].department);
                free(emps[ei].title);
                emps[ei].name = strdup(a2);
                emps[ei].department = strdup(a3);
                emps[ei].title = strdup(a4);
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            int ei = find_emp(a1);
            if (ei >= 0) {
                free(emps[ei].id);
                free(emps[ei].name);
                free(emps[ei].department);
                free(emps[ei].title);
                for (int k = ei; k < ec - 1; k++) {
                    emps[k] = emps[k + 1];
                }
                ec--;
            }
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            int ei = find_emp(a1);
            if (ei >= 0) {
                printf("%s %s %s %s\n", emps[ei].id, emps[ei].name, emps[ei].department, emps[ei].title);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < ec; i++) {
                printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].department, emps[i].title);
            }
        }
    }
    for (int i = 0; i < ec; i++) {
        free(emps[i].id);
        free(emps[i].name);
        free(emps[i].department);
        free(emps[i].title);
    }
    free(emps);
    return 0;
}