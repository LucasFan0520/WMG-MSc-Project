// F022.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
    int is_done;
} Task;

typedef struct {
    char *name;
    Task *tasks;
    int task_count;
} Project;

int main(void) {
    Project *projects = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            Project *tmp = realloc(projects, sizeof(Project) * (count + 1));
            if (tmp) {
                projects = tmp;
                projects[count].name = strdup(name);
                projects[count].tasks = NULL;
                projects[count].task_count = 0;
                if (projects[count].name) {
                    count++;
                }
            }
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *p = line + 5;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *p_name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *title = p2;
                    char *note = s2 + 1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, p_name) == 0) {
                            Task *ttmp = realloc(projects[i].tasks, sizeof(Task) * (projects[i].task_count + 1));
                            if (ttmp) {
                                projects[i].tasks = ttmp;
                                projects[i].tasks[projects[i].task_count].title = strdup(title);
                                projects[i].tasks[projects[i].task_count].note = strdup(note);
                                projects[i].tasks[projects[i].task_count].is_done = 0;
                                if (projects[i].tasks[projects[i].task_count].title && projects[i].tasks[projects[i].task_count].note) {
                                    projects[i].task_count++;
                                }
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *p = line + 9;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *fromP = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *toP = p2;
                    char *title = s2 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, fromP) == 0) f_idx = i;
                        if (strcmp(projects[i].name, toP) == 0) t_idx = i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        int t_found = -1;
                        for (int j = 0; j < projects[f_idx].task_count; j++) {
                            if (strcmp(projects[f_idx].tasks[j].title, title) == 0) {
                                t_found = j;
                                break;
                            }
                        }
                        if (t_found != -1) {
                            Task *ttmp = realloc(projects[t_idx].tasks, sizeof(Task) * (projects[t_idx].task_count + 1));
                            if (ttmp) {
                                projects[t_idx].tasks = ttmp;
                                projects[t_idx].tasks[projects[t_idx].task_count] = projects[f_idx].tasks[t_found];
                                projects[t_idx].task_count++;
                                for (int j = t_found; j < projects[f_idx].task_count - 1; j++) {
                                    projects[f_idx].tasks[j] = projects[f_idx].tasks[j + 1];
                                }
                                projects[f_idx].task_count--;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *p = line + 9;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *p_name = p;
                char *title = s1 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, p_name) == 0) {
                        for (int j = 0; j < projects[i].task_count; j++) {
                            if (strcmp(projects[i].tasks[j].title, title) == 0) {
                                projects[i].tasks[j].is_done = 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *name = line + 14;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(projects[idx].name);
                for (int j = 0; j < projects[idx].task_count; j++) {
                    free(projects[idx].tasks[j].title);
                    free(projects[idx].tasks[j].note);
                }
                free(projects[idx].tasks);
                for (int i = idx; i < count - 1; i++) {
                    projects[i] = projects[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("  Task: %s [%s] - %s\n", projects[i].tasks[j].title, projects[i].tasks[j].is_done ? "DONE" : "TODO", projects[i].tasks[j].note);
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
    free(line);
    return 0;
}
