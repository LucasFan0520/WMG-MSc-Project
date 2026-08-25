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
    int tcount;
} Project;

Project *projects = NULL;
int pcount = 0;

int find_project(const char *name) {
    for (int i = 0; i < pcount; i++) {
        if (strcmp(projects[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "PROJECT") == 0) {
            char name[100];
            scanf("%s", name);
            if (find_project(name) == -1) {
                projects = realloc(projects, sizeof(Project) * (pcount + 1));
                projects[pcount].name = strdup(name);
                projects[pcount].tasks = NULL;
                projects[pcount].tcount = 0;
                pcount++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char proj[100], title[100], note[1000];
            scanf("%s %s", proj, title);
            scanf(" %[^\n]", note);
            int idx = find_project(proj);
            if (idx != -1) {
                projects[idx].tasks = realloc(projects[idx].tasks, sizeof(Task) * (projects[idx].tcount + 1));
                projects[idx].tasks[projects[idx].tcount].title = strdup(title);
                projects[idx].tasks[projects[idx].tcount].note = strdup(note);
                projects[idx].tasks[projects[idx].tcount].done = 0;
                projects[idx].tcount++;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[100], to[100], title[100];
            scanf("%s %s %s", from, to, title);
            int fi = find_project(from);
            int ti = find_project(to);
            if (fi != -1 && ti != -1) {
                for (int i = 0; i < projects[fi].tcount; i++) {
                    if (strcmp(projects[fi].tasks[i].title, title) == 0) {
                        projects[ti].tasks = realloc(projects[ti].tasks, sizeof(Task) * (projects[ti].tcount + 1));
                        projects[ti].tasks[projects[ti].tcount] = projects[fi].tasks[i];
                        projects[ti].tcount++;
                        for (int j = i; j < projects[fi].tcount - 1; j++) {
                            projects[fi].tasks[j] = projects[fi].tasks[j+1];
                        }
                        projects[fi].tcount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char proj[100], title[100];
            scanf("%s %s", proj, title);
            int idx = find_project(proj);
            if (idx != -1) {
                for (int i = 0; i < projects[idx].tcount; i++) {
                    if (strcmp(projects[idx].tasks[i].title, title) == 0) {
                        projects[idx].tasks[i].done = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_project(name);
            if (idx != -1) {
                free(projects[idx].name);
                for (int i = 0; i < projects[idx].tcount; i++) {
                    free(projects[idx].tasks[i].title);
                    free(projects[idx].tasks[i].note);
                }
                free(projects[idx].tasks);
                for (int i = idx; i < pcount - 1; i++) projects[i] = projects[i+1];
                pcount--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < pcount; i++) {
                printf("%s\n", projects[i].name);
                for (int j = 0; j < projects[i].tcount; j++) {
                    printf("  %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
        }
    }
    for (int i = 0; i < pcount; i++) {
        free(projects[i].name);
        for (int j = 0; j < projects[i].tcount; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}