// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int done;
} Task;

typedef struct {
    char *name;
    Task *tasks;
    int tc;
} Project;

Project *projects = NULL;
int pc = 0;

int find_project(const char *name) {
    for (int i = 0; i < pc; i++) {
        if (strcmp(projects[i].name, name) == 0) return i;
    }
    return -1;
}

int find_task(int pi, const char *title) {
    for (int j = 0; j < projects[pi].tc; j++) {
        if (strcmp(projects[pi].tasks[j].title, title) == 0) return j;
    }
    return -1;
}

int main(void) {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[256];
        char a3[256];
        char rest[1024];
        a1[0] = '\0';
        a2[0] = '\0';
        a3[0] = '\0';
        rest[0] = '\0';
        int n = sscanf(line, "%31s %255s %255s %255s %1023[^\n]", cmd, a1, a2, a3, rest);
        if (n < 1) continue;

        if (strcmp(cmd, "PROJECT") == 0 && n >= 2) {
            if (find_project(a1) < 0) {
                projects = realloc(projects, (pc + 1) * sizeof(Project));
                if (!projects) return 1;
                projects[pc].name = strdup(a1);
                projects[pc].tasks = NULL;
                projects[pc].tc = 0;
                pc++;
            }
        } else if (strcmp(cmd, "TASK") == 0 && n >= 4) {
            int pi = find_project(a1);
            if (pi >= 0) {
                projects[pi].tasks = realloc(projects[pi].tasks, (projects[pi].tc + 1) * sizeof(Task));
                if (!projects[pi].tasks) return 1;
                projects[pi].tasks[projects[pi].tc].title = strdup(a2);
                projects[pi].tasks[projects[pi].tc].note = strdup(a3);
                projects[pi].tasks[projects[pi].tc].done = 0;
                projects[pi].tc++;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0 && n >= 4) {
            int from = find_project(a1);
            int to = find_project(a2);
            if (from >= 0 && to >= 0) {
                int ti = find_task(from, a3);
                if (ti >= 0) {
                    projects[to].tasks = realloc(projects[to].tasks, (projects[to].tc + 1) * sizeof(Task));
                    if (!projects[to].tasks) return 1;
                    projects[to].tasks[projects[to].tc] = projects[from].tasks[ti];
                    projects[to].tc++;
                    for (int k = ti; k < projects[from].tc - 1; k++) {
                        projects[from].tasks[k] = projects[from].tasks[k + 1];
                    }
                    projects[from].tc--;
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0 && n >= 3) {
            int pi = find_project(a1);
            if (pi >= 0) {
                int ti = find_task(pi, a2);
                if (ti >= 0) {
                    projects[pi].tasks[ti].done = 1;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0 && n >= 2) {
            int pi = find_project(a1);
            if (pi >= 0) {
                for (int j = 0; j < projects[pi].tc; j++) {
                    free(projects[pi].tasks[j].title);
                    free(projects[pi].tasks[j].note);
                }
                free(projects[pi].tasks);
                free(projects[pi].name);
                for (int k = pi; k < pc - 1; k++) {
                    projects[k] = projects[k + 1];
                }
                pc--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < pc; i++) {
                printf("%s\n", projects[i].name);
                for (int j = 0; j < projects[i].tc; j++) {
                    printf("  %s %s %d\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done);
                }
            }
        }
    }
    for (int i = 0; i < pc; i++) {
        for (int j = 0; j < projects[i].tc; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
        free(projects[i].name);
    }
    free(projects);
    return 0;
}