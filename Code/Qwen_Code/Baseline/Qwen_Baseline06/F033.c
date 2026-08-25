// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id, *name, *dept, *title; } Emp;
Emp *emps; int n_emp;

int find_emp(char *id) {
    for(int i=0; i<n_emp; i++) if(!strcmp(emps[i].id, id)) return i;
    return -1;
}

int main() {
    char cmd[20], id[100], name[100], dept[100], title[100];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "ADD")) {
            scanf("%s %s %s %s", id, name, dept, title);
            emps = realloc(emps, (n_emp+1)*sizeof(Emp));
            emps[n_emp].id = strdup(id);
            emps[n_emp].name = strdup(name);
            emps[n_emp].dept = strdup(dept);
            emps[n_emp].title = strdup(title);
            n_emp++;
        } else if(!strcmp(cmd, "UPDATE")) {
            scanf("%s %s %s %s", id, name, dept, title);
            int idx = find_emp(id);
            if(idx != -1) {
                free(emps[idx].name); free(emps[idx].dept); free(emps[idx].title);
                emps[idx].name = strdup(name);
                emps[idx].dept = strdup(dept);
                emps[idx].title = strdup(title);
            }
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", id);
            int idx = find_emp(id);
            if(idx != -1) {
                free(emps[idx].id); free(emps[idx].name); free(emps[idx].dept); free(emps[idx].title);
                for(int k=idx; k<n_emp-1; k++) emps[k] = emps[k+1];
                n_emp--;
            }
        } else if(!strcmp(cmd, "FIND")) {
            scanf("%s", id);
            int idx = find_emp(id);
            if(idx != -1) printf("%s %s %s %s\n", emps[idx].id, emps[idx].name, emps[idx].dept, emps[idx].title);
        } else if(!strcmp(cmd, "LIST")) {
            for(int i=0; i<n_emp; i++) printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
        }
    }
    return 0;
}