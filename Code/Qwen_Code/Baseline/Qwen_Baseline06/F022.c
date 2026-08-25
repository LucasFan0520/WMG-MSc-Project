// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *note; int done; } Task;
typedef struct { char *name; Task *tasks; int n_task; } Proj;
Proj *projs; int n_proj;

int find_proj(char *name) {
    for(int i=0; i<n_proj; i++) if(!strcmp(projs[i].name, name)) return i;
    return -1;
}

int find_task(Proj *p, char *title) {
    for(int i=0; i<p->n_task; i++) if(!strcmp(p->tasks[i].title, title)) return i;
    return -1;
}

int main() {
    char cmd[20], p1[100], p2[100], title[100], note[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "PROJECT")) {
            scanf("%s", p1);
            projs = realloc(projs, (n_proj+1)*sizeof(Proj));
            projs[n_proj].name = strdup(p1);
            projs[n_proj].tasks = NULL;
            projs[n_proj].n_task = 0;
            n_proj++;
        } else if(!strcmp(cmd, "TASK")) {
            scanf("%s %s", p1, title);
            fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            int i = find_proj(p1);
            if(i != -1) {
                projs[i].tasks = realloc(projs[i].tasks, (projs[i].n_task+1)*sizeof(Task));
                projs[i].tasks[projs[i].n_task].title = strdup(title);
                projs[i].tasks[projs[i].n_task].note = strdup(note);
                projs[i].tasks[projs[i].n_task].done = 0;
                projs[i].n_task++;
            }
        } else if(!strcmp(cmd, "MOVETASK")) {
            scanf("%s %s %s", p1, p2, title);
            int i1 = find_proj(p1), i2 = find_proj(p2);
            if(i1 != -1 && i2 != -1) {
                int t = find_task(&projs[i1], title);
                if(t != -1) {
                    projs[i2].tasks = realloc(projs[i2].tasks, (projs[i2].n_task+1)*sizeof(Task));
                    projs[i2].tasks[projs[i2].n_task] = projs[i1].tasks[t];
                    projs[i2].n_task++;
                    for(int k=t; k<projs[i1].n_task-1; k++) projs[i1].tasks[k] = projs[i1].tasks[k+1];
                    projs[i1].n_task--;
                }
            }
        } else if(!strcmp(cmd, "DONETASK")) {
            scanf("%s %s", p1, title);
            int i = find_proj(p1);
            if(i != -1) {
                int t = find_task(&projs[i], title);
                if(t != -1) projs[i].tasks[t].done = 1;
            }
        } else if(!strcmp(cmd, "DELETEPROJECT")) {
            scanf("%s", p1);
            int i = find_proj(p1);
            if(i != -1) {
                free(projs[i].name);
                for(int j=0; j<projs[i].n_task; j++) {
                    free(projs[i].tasks[j].title);
                    free(projs[i].tasks[j].note);
                }
                free(projs[i].tasks);
                for(int k=i; k<n_proj-1; k++) projs[k] = projs[k+1];
                n_proj--;
            }
        } else if(!strcmp(cmd, "REPORT")) {
            for(int i=0; i<n_proj; i++) {
                printf("%s\n", projs[i].name);
                for(int j=0; j<projs[i].n_task; j++) {
                    printf("%s %s %s\n", projs[i].tasks[j].title, projs[i].tasks[j].done?"DONE":"TODO", projs[i].tasks[j].note);
                }
            }
        }
    }
    return 0;
}