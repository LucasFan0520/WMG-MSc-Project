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

Project projects[1000];
int pc = 0;

int find_project(char *name) {
    for (int i = 0; i < pc; i++) if (strcmp(projects[i].name, name) == 0) return i;
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
        
        if (strcmp(cmd, "PROJECT") == 0) {
            if (find_project(rest) == -1) {
                projects[pc].name = strdup(rest);
                projects[pc].tasks = NULL;
                projects[pc].tc = 0;
                pc++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char proj[1000], title[1000];
            sscanf(rest, "%s %s", proj, title);
            char *note = rest;
            while (*note && *note != ' ') note++;
            while (*note == ' ') note++;
            while (*note && *note != ' ') note++;
            while (*note == ' ') note++;
            int idx = find_project(proj);
            if (idx != -1) {
                projects[idx].tasks = realloc(projects[idx].tasks, sizeof(Task) * (projects[idx].tc + 1));
                projects[idx].tasks[projects[idx].tc].title = strdup(title);
                projects[idx].tasks[projects[idx].tc].note = strdup(note);
                projects[idx].tasks[projects[idx].tc].done = 0;
                projects[idx].tc++;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[1000], to[1000], title[1000];
            sscanf(rest, "%s %s %s", from, to, title);
            int ifr = find_project(from), ito = find_project(to);
            if (ifr != -1 && ito != -1) {
                for (int i = 0; i < projects[ifr].tc; i++) {
                    if (strcmp(projects[ifr].tasks[i].title, title) == 0) {
                        projects[ito].tasks = realloc(projects[ito].tasks, sizeof(Task) * (projects[ito].tc + 1));
                        projects[ito].tasks[projects[ito].tc] = projects[ifr].tasks[i];
                        projects[ito].tc++;
                        for (int j = i; j < projects[ifr].tc - 1; j++) projects[ifr].tasks[j] = projects[ifr].tasks[j+1];
                        projects[ifr].tc--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char proj[1000], title[1000];
            sscanf(rest, "%s %s", proj, title);
            int idx = find_project(proj);
            if (idx != -1) {
                for (int i = 0; i < projects[idx].tc; i++) {
                    if (strcmp(projects[idx].tasks[i].title, title) == 0) {
                        projects[idx].tasks[i].done = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            int idx = find_project(rest);
            if (idx != -1) {
                free(projects[idx].name);
                for (int i = 0; i < projects[idx].tc; i++) {
                    free(projects[idx].tasks[i].title);
                    free(projects[idx].tasks[i].note);
                }
                free(projects[idx].tasks);
                for (int i = idx; i < pc - 1; i++) projects[i] = projects[i+1];
                pc--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < pc; i++) {
                printf("%s\n", projects[i].name);
                for (int j = 0; j < projects[i].tc; j++) {
                    printf("  %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
        }
    }
    for (int i = 0; i < pc; i++) {
        free(projects[i].name);
        for (int j = 0; j < projects[i].tc; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    return 0;
}