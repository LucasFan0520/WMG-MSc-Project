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
    int task_count;
} Project;

int main() {
    Project *projects = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                projects = realloc(projects, (count + 1) * sizeof(Project));
                projects[count].name = strdup(name);
                projects[count].tasks = NULL;
                projects[count].task_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *pname = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (pname && title && note) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, pname) == 0) {
                        projects[i].tasks = realloc(projects[i].tasks, (projects[i].task_count + 1) * sizeof(Task));
                        projects[i].tasks[projects[i].task_count].title = strdup(title);
                        projects[i].tasks[projects[i].task_count].note = strdup(note);
                        projects[i].tasks[projects[i].task_count].done = 0;
                        projects[i].task_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *fromP = strtok(NULL, " ");
            char *toP = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (fromP && toP && title) {
                int idxF = -1, idxT = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, fromP) == 0) idxF = i;
                    if (strcmp(projects[i].name, toP) == 0) idxT = i;
                }
                if (idxF != -1 && idxT != -1) {
                    for (int j = 0; j < projects[idxF].task_count; j++) {
                        if (strcmp(projects[idxF].tasks[j].title, title) == 0) {
                            projects[idxT].tasks = realloc(projects[idxT].tasks, (projects[idxT].task_count + 1) * sizeof(Task));
                            projects[idxT].tasks[projects[idxT].task_count].title = strdup(projects[idxF].tasks[j].title);
                            projects[idxT].tasks[projects[idxT].task_count].note = strdup(projects[idxF].tasks[j].note);
                            projects[idxT].tasks[projects[idxT].task_count].done = projects[idxF].tasks[j].done;
                            projects[idxT].task_count++;
                            free(projects[idxF].tasks[j].title);
                            free(projects[idxF].tasks[j].note);
                            for (int k = j; k < projects[idxF].task_count - 1; k++) {
                                projects[idxF].tasks[k] = projects[idxF].tasks[k + 1];
                            }
                            projects[idxF].task_count--;
                            if (projects[idxF].task_count == 0) {
                                free(projects[idxF].tasks);
                                projects[idxF].tasks = NULL;
                            } else {
                                projects[idxF].tasks = realloc(projects[idxF].tasks, projects[idxF].task_count * sizeof(Task));
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *pname = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (pname && title) {
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
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, name) == 0) {
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
                        i--;
                    }
                }
                if (count == 0) {
                    free(projects);
                    projects = NULL;
                } else {
                    projects = realloc(projects, count * sizeof(Project));
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("  Task: %s [%s] - %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
    }
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
