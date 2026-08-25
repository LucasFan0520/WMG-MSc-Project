// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *note; int done; } Task;
typedef struct { char *name; Task *tasks; int ntasks; } Project;
Project projects[1000]; int nprojects = 0;

int find_proj(char *name) {
    for (int i = 0; i < nprojects; i++) if (!strcmp(projects[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], title[100], note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "PROJECT")) {
            scanf("%s", name);
            projects[nprojects].name = strdup(name);
            projects[nprojects].tasks = NULL;
            projects[nprojects++].ntasks = 0;
        } else if (!strcmp(cmd, "TASK")) {
            scanf("%s", name);
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            char *space = strchr(title, ' ');
            if (space) {
                *space = 0;
                char *n = space + 1;
                while(*n == ' ') n++;
                int idx = find_proj(name);
                if (idx != -1) {
                    projects[idx].tasks = realloc(projects[idx].tasks, sizeof(Task) * (projects[idx].ntasks + 1));
                    projects[idx].tasks[projects[idx].ntasks].title = strdup(title);
                    projects[idx].tasks[projects[idx].ntasks].note = strdup(n);
                    projects[idx].tasks[projects[idx].ntasks++].done = 0;
                }
            }
        } else if (!strcmp(cmd, "MOVETASK")) {
            char from[100], to[100];
            scanf("%s %s", from, to);
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            int ifrom = find_proj(from), ito = find_proj(to);
            if (ifrom != -1 && ito != -1) {
                for (int i = 0; i < projects[ifrom].ntasks; i++) {
                    if (!strcmp(projects[ifrom].tasks[i].title, title)) {
                        projects[ito].tasks = realloc(projects[ito].tasks, sizeof(Task) * (projects[ito].ntasks + 1));
                        projects[ito].tasks[projects[ito].ntasks++] = projects[ifrom].tasks[i];
                        for (int j = i; j < projects[ifrom].ntasks - 1; j++) projects[ifrom].tasks[j] = projects[ifrom].tasks[j + 1];
                        projects[ifrom].ntasks--;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DONETASK")) {
            scanf("%s", name);
            fgets(title, sizeof(title), stdin);
            title[strcspn(title, "\n")] = 0;
            if (title[0] == ' ') memmove(title, title + 1, strlen(title));
            int idx = find_proj(name);
            if (idx != -1) {
                for (int i = 0; i < projects[idx].ntasks; i++) {
                    if (!strcmp(projects[idx].tasks[i].title, title)) {
                        projects[idx].tasks[i].done = 1;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETEPROJECT")) {
            scanf("%s", name);
            int idx = find_proj(name);
            if (idx != -1) {
                free(projects[idx].name);
                for (int i = 0; i < projects[idx].ntasks; i++) {
                    free(projects[idx].tasks[i].title);
                    free(projects[idx].tasks[i].note);
                }
                free(projects[idx].tasks);
                for (int i = idx; i < nprojects - 1; i++) projects[i] = projects[i + 1];
                nprojects--;
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < nprojects; i++) {
                printf("%s\n", projects[i].name);
                for (int j = 0; j < projects[i].ntasks; j++) {
                    printf("  %s %s %s\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
        }
    }
    for (int i = 0; i < nprojects; i++) {
        free(projects[i].name);
        for (int j = 0; j < projects[i].ntasks; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    return 0;
}