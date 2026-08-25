// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROJ 1000
#define MAX_TASK 1000
#define MAX_LEN 1000

typedef struct {
    char title[100];
    char note[MAX_LEN];
    int done;
} Task;

typedef struct {
    char name[100];
    Task tasks[MAX_TASK];
    int tcount;
} Project;

Project projects[MAX_PROJ];
int pcount = 0;

int find_proj(const char *name) {
    for (int i = 0; i < pcount; i++) if (strcmp(projects[i].name, name) == 0) return i;
    return -1;
}

int find_task(int pidx, const char *title) {
    for (int i = 0; i < projects[pidx].tcount; i++) {
        if (strcmp(projects[pidx].tasks[i].title, title) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], p1[100], p2[100], title[100], note[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "PROJECT") == 0) {
            scanf("%s", p1);
            if (find_proj(p1) == -1 && pcount < MAX_PROJ) {
                strcpy(projects[pcount].name, p1);
                projects[pcount].tcount = 0;
                pcount++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            scanf("%s %s", p1, title);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            int pidx = find_proj(p1);
            if (pidx != -1 && projects[pidx].tcount < MAX_TASK) {
                strcpy(projects[pidx].tasks[projects[pidx].tcount].title, title);
                strcpy(projects[pidx].tasks[projects[pidx].tcount].note, note);
                projects[pidx].tasks[projects[pidx].tcount].done = 0;
                projects[pidx].tcount++;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            scanf("%s %s %s", p1, p2, title);
            int i1 = find_proj(p1);
            int i2 = find_proj(p2);
            if (i1 != -1 && i2 != -1) {
                int tidx = find_task(i1, title);
                if (tidx != -1 && projects[i2].tcount < MAX_TASK) {
                    projects[i2].tasks[projects[i2].tcount] = projects[i1].tasks[tidx];
                    projects[i2].tcount++;
                    for (int i = tidx; i < projects[i1].tcount - 1; i++) {
                        projects[i1].tasks[i] = projects[i1].tasks[i + 1];
                    }
                    projects[i1].tcount--;
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            scanf("%s %s", p1, title);
            int pidx = find_proj(p1);
            if (pidx != -1) {
                int tidx = find_task(pidx, title);
                if (tidx != -1) projects[pidx].tasks[tidx].done = 1;
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            scanf("%s", p1);
            int pidx = find_proj(p1);
            if (pidx != -1) {
                for (int i = pidx; i < pcount - 1; i++) projects[i] = projects[i + 1];
                pcount--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < pcount; i++) {
                printf("%s\n", projects[i].name);
                for (int j = 0; j < projects[i].tcount; j++) {
                    printf("  %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "done" : "pending", projects[i].tasks[j].note);
                }
            }
        }
    }
    return 0;
}