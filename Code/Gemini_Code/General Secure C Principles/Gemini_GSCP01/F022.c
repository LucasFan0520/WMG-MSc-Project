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
    size_t task_count;
} Project;

int main(void) {
    Project *projects = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Project *temp = realloc(projects, (count + 1) * sizeof(Project));
                if (temp) {
                    projects = temp;
                    projects[count].name = strdup(name);
                    projects[count].tasks = NULL;
                    projects[count].task_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *proj = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
                    ProjectTask *temp = realloc(projects[i].tasks, (projects[i].task_count + 1) * sizeof(ProjectTask));
                    if (temp) {
                        projects[i].tasks = temp;
                        projects[i].tasks[projects[i].task_count].title = strdup(title);
                        projects[i].tasks[projects[i].task_count].note = strdup(note);
                        projects[i].tasks[projects[i].task_count].done = 0;
                        projects[i].task_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *fromProj = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *toProj = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int from_idx = -1, to_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, fromProj) == 0) from_idx = (int)i;
                if (strcmp(projects[i].name, toProj) == 0) to_idx = (int)i;
            }
            if (from_idx != -1 && to_idx != -1) {
                for (size_t k = 0; k < projects[from_idx].task_count; k++) {
                    if (strcmp(projects[from_idx].tasks[k].title, title) == 0) {
                        ProjectTask *temp = realloc(projects[to_idx].tasks, (projects[to_idx].task_count + 1) * sizeof(ProjectTask));
                        if (temp) {
                            projects[to_idx].tasks = temp;
                            projects[to_idx].tasks[projects[to_idx].task_count] = projects[from_idx].tasks[k];
                            projects[to_idx].task_count++;
                            for (size_t j = k; j < projects[from_idx].task_count - 1; j++) {
                                projects[from_idx].tasks[j] = projects[from_idx].tasks[j + 1];
                            }
                            projects[from_idx].task_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *proj = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
                    for (size_t k = 0; k < projects[i].task_count; k++) {
                        if (strcmp(projects[i].tasks[k].title, title) == 0) {
                            projects[i].tasks[k].done = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    free(projects[i].name);
                    for (size_t k = 0; k < projects[i].task_count; k++) {
                        free(projects[i].tasks[k].title);
                        free(projects[i].tasks[k].note);
                    }
                    free(projects[i].tasks);
                    for (size_t j = i; j < count - 1; j++) {
                        projects[j] = projects[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (size_t k = 0; k < projects[i].task_count; k++) {
                    printf("  Task: %s [%s] - %s\n", projects[i].tasks[k].title, projects[i].tasks[k].done ? "DONE" : "PENDING", projects[i].tasks[k].note);
                }
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(projects[i].name);
        for (size_t k = 0; k < projects[i].task_count; k++) {
            free(projects[i].tasks[k].title);
            free(projects[i].tasks[k].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    free(line);
    return 0;
}
