// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *note; int done; } Task;
typedef struct { char *name; Task *tasks; int n; } Proj;
Proj *projs = NULL;
int np = 0;

int find_proj(const char *name) {
    for (int i = 0; i < np; i++) if (strcmp(projs[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256], b[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "PROJECT") == 0) {
            sscanf(line, "%*s %s", a);
            if (find_proj(a) == -1) {
                projs = realloc(projs, (np + 1) * sizeof(Proj));
                projs[np].name = strdup(a);
                projs[np].tasks = NULL;
                projs[np].n = 0;
                np++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            char *note = strchr(t, ' ');
            char title[256];
            strncpy(title, t, note ? note - t : strlen(t));
            title[note ? note - t : strlen(t)] = 0;
            
            int idx = find_proj(a);
            if (idx != -1) {
                projs[idx].tasks = realloc(projs[idx].tasks, (projs[idx].n + 1) * sizeof(Task));
                projs[idx].tasks[projs[idx].n].title = strdup(title);
                projs[idx].tasks[projs[idx].n].note = strdup(note ? note + 1 : "");
                projs[idx].tasks[projs[idx].n].done = 0;
                projs[idx].n++;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++; t = strchr(t, ' '); t++;
            int from = find_proj(a), to = find_proj(b);
            if (from != -1 && to != -1) {
                for (int i = 0; i < projs[from].n; i++) {
                    if (strcmp(projs[from].tasks[i].title, t) == 0) {
                        projs[to].tasks = realloc(projs[to].tasks, (projs[to].n + 1) * sizeof(Task));
                        projs[to].tasks[projs[to].n] = projs[from].tasks[i];
                        projs[to].n++;
                        for (int j = i; j < projs[from].n - 1; j++) projs[from].tasks[j] = projs[from].tasks[j + 1];
                        projs[from].n--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            int idx = find_proj(a);
            if (idx != -1) {
                for (int i = 0; i < projs[idx].n; i++) {
                    if (strcmp(projs[idx].tasks[i].title, t) == 0) {
                        projs[idx].tasks[i].done = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            sscanf(line, "%*s %s", a);
            int idx = find_proj(a);
            if (idx != -1) {
                for (int i = 0; i < projs[idx].n; i++) {
                    free(projs[idx].tasks[i].title);
                    free(projs[idx].tasks[i].note);
                }
                free(projs[idx].tasks);
                free(projs[idx].name);
                for (int i = idx; i < np - 1; i++) projs[i] = projs[i + 1];
                np--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < np; i++) {
                printf("%s:\n", projs[i].name);
                for (int j = 0; j < projs[i].n; j++) {
                    printf("  %s [%s] %s\n", projs[i].tasks[j].title, projs[i].tasks[j].done ? "done" : "pending", projs[i].tasks[j].note);
                }
            }
        }
    }
    for (int i = 0; i < np; i++) {
        for (int j = 0; j < projs[i].n; j++) {
            free(projs[i].tasks[j].title);
            free(projs[i].tasks[j].note);
        }
        free(projs[i].tasks);
        free(projs[i].name);
    }
    free(projs);
    return 0;
}