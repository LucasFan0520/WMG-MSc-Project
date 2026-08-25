// F022.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int done;
} ProjectTask;

typedef struct {
    char *name;
    ProjectTask *tasks;
    int task_count;
} Project;

int main() {
    Project *projects = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "PROJECT") == 0 && args) {
            projects = realloc(projects, (count + 1) * sizeof(Project));
            if (!projects) return 1;
            projects[count].name = strdup(args);
            projects[count].tasks = NULL;
            projects[count].task_count = 0;
            count++;
        } else if (strcmp(cmd, "TASK") == 0 && args) {
            char *pname = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *title = space2 + 1;
                while (*title == ' ') title++;
                char *space3 = strchr(title, ' ');
                char *note = "";
                if (space3) {
                    *space3 = '\0';
                    note = space3 + 1;
                    while (*note == ' ') note++;
                }
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, pname) == 0) {
                        int tc = projects[i].task_count;
                        projects[i].tasks = realloc(projects[i].tasks, (tc + 1) * sizeof(ProjectTask));
                        if (!projects[i].tasks) return 1;
                        projects[i].tasks[tc].title = strdup(title);
                        projects[i].tasks[tc].note = strdup(note);
                        projects[i].tasks[tc].done = 0;
                        projects[i].task_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0 && args) {
            char *fromP = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *toP = space2 + 1;
                while (*toP == ' ') toP++;
                char *space3 = strchr(toP, ' ');
                if (space3) {
                    *space3 = '\0';
                    char *title = space3 + 1;
                    while (*title == ' ') title++;
                    int f_idx = -1, t_idx = -1, tk_idx = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, fromP) == 0) f_idx = i;
                        if (strcmp(projects[i].name, toP) == 0) t_idx = i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        for (int j = 0; j < projects[f_idx].task_count; j++) {
                            if (strcmp(projects[f_idx].tasks[j].title, title) == 0) {
                                tk_idx = j;
                                break;
                            }
                        }
                        if (tk_idx != -1) {
                            int tc = projects[t_idx].task_count;
                            projects[t_idx].tasks = realloc(projects[t_idx].tasks, (tc + 1) * sizeof(ProjectTask));
                            if (!projects[t_idx].tasks) return 1;
                            projects[t_idx].tasks[tc] = projects[f_idx].tasks[tk_idx];
                            projects[t_idx].task_count++;

                            for (int j = tk_idx; j < projects[f_idx].task_count - 1; j++) {
                                projects[f_idx].tasks[j] = projects[f_idx].tasks[j + 1];
                            }
                            projects[f_idx].task_count--;
                            projects[f_idx].tasks = realloc(projects[f_idx].tasks, projects[f_idx].task_count * sizeof(ProjectTask));
                            if (projects[f_idx].task_count > 0 && !projects[f_idx].tasks) return 1;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0 && args) {
            char *pname = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *title = space2 + 1;
                while (*title == ' ') title++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, pname) == 0) {
                        for (int j = 0; j < projects[i].task_count; j++) {
                            if (strcmp(projects[i].tasks[j].title, title) == 0) {
                                projects[i].tasks[j].done = 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(projects[i].name, args) == 0) {
                    free(projects[i].name);
                    for (int j = 0; j < projects[i].task_count; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    for (int j = i; j < count - 1; j++) {
                        projects[j] = projects[j + 1];
                    }
                    count--;
                    projects = realloc(projects, count * sizeof(Project));
                    if (count > 0 && !projects) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("PROJECT %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("TASK %s [%s] %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(projects[i].name);
        for (int j = 0; j < projects[i].task_count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
