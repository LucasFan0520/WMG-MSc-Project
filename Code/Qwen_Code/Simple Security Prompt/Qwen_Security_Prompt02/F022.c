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
    int count;
    int cap;
} Project;

Project *projects = NULL;
int p_count = 0;

int find_project(const char *name) {
    for (int i = 0; i < p_count; i++) {
        if (strcmp(projects[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_task(int pi, const char *title) {
    for (int i = 0; i < projects[pi].count; i++) {
        if (strcmp(projects[pi].tasks[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char name[1024];
    char title[1024];
    char from[1024];
    char to[1024];
    char note[10240];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "PROJECT") == 0) {
            scanf("%s", name);
            if (find_project(name) == -1) {
                projects = realloc(projects, sizeof(Project) * (p_count + 1));
                projects[p_count].name = strdup(name);
                projects[p_count].tasks = NULL;
                projects[p_count].count = 0;
                projects[p_count].cap = 0;
                p_count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            scanf("%s %s", name, title);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = '\0';
            int pi = find_project(name);
            if (pi != -1) {
                if (projects[pi].count == projects[pi].cap) {
                    projects[pi].cap = projects[pi].cap ? projects[pi].cap * 2 : 4;
                    projects[pi].tasks = realloc(projects[pi].tasks, sizeof(Task) * projects[pi].cap);
                }
                projects[pi].tasks[projects[pi].count].title = strdup(title);
                projects[pi].tasks[projects[pi].count].note = strdup(note);
                projects[pi].tasks[projects[pi].count].done = 0;
                projects[pi].count++;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            scanf("%s %s %s", from, to, title);
            int fi = find_project(from);
            int ti = find_project(to);
            if (fi != -1 && ti != -1) {
                int tki = find_task(fi, title);
                if (tki != -1) {
                    if (projects[ti].count == projects[ti].cap) {
                        projects[ti].cap = projects[ti].cap ? projects[ti].cap * 2 : 4;
                        projects[ti].tasks = realloc(projects[ti].tasks, sizeof(Task) * projects[ti].cap);
                    }
                    projects[ti].tasks[projects[ti].count] = projects[fi].tasks[tki];
                    projects[ti].count++;
                    for (int i = tki; i < projects[fi].count - 1; i++) {
                        projects[fi].tasks[i] = projects[fi].tasks[i + 1];
                    }
                    projects[fi].count--;
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            scanf("%s %s", name, title);
            int pi = find_project(name);
            if (pi != -1) {
                int ti = find_task(pi, title);
                if (ti != -1) {
                    projects[pi].tasks[ti].done = 1;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            scanf("%s", name);
            int pi = find_project(name);
            if (pi != -1) {
                free(projects[pi].name);
                for (int i = 0; i < projects[pi].count; i++) {
                    free(projects[pi].tasks[i].title);
                    free(projects[pi].tasks[i].note);
                }
                free(projects[pi].tasks);
                for (int i = pi; i < p_count - 1; i++) {
                    projects[i] = projects[i + 1];
                }
                p_count--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < p_count; i++) {
                printf("%s\n", projects[i].name);
                for (int j = 0; j < projects[i].count; j++) {
                    printf("  %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "done" : "open", projects[i].tasks[j].note);
                }
            }
        }
    }

    for (int i = 0; i < p_count; i++) {
        free(projects[i].name);
        for (int j = 0; j < projects[i].count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);

    return 0;
}