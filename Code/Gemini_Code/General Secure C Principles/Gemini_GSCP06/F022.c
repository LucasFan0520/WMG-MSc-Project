// F022.c
#define _GNU_SOURCE
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
    size_t task_count;
    size_t task_capacity;
} Project;

int main(void) {
    Project *projects = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Project *next = realloc(projects, capacity * sizeof(Project));
                if (!next) break;
                projects = next;
            }
            projects[count].name = strdup(name);
            projects[count].tasks = NULL;
            projects[count].task_count = 0;
            projects[count].task_capacity = 0;
            count++;
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *args = line + 5;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *pname = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *title = rest;
                    char *note = sp2 + 1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, pname) == 0) {
                            if (projects[i].task_count >= projects[i].task_capacity) {
                                projects[i].task_capacity = projects[i].task_capacity == 0 ? 4 : projects[i].task_capacity * 2;
                                Task *nxt = realloc(projects[i].tasks, projects[i].task_capacity * sizeof(Task));
                                if (!nxt) break;
                                projects[i].tasks = nxt;
                            }
                            projects[i].tasks[projects[i].task_count].title = strdup(title);
                            projects[i].tasks[projects[i].task_count].note = strdup(note);
                            projects[i].tasks[projects[i].task_count].done = 0;
                            projects[i].task_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *args = line + 9;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *fromP = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    char *toP = rest;
                    char *title = sp2 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, fromP) == 0) f_idx = (int)i;
                        if (strcmp(projects[i].name, toP) == 0) t_idx = (int)i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        for (size_t k = 0; k < projects[f_idx].task_count; k++) {
                            if (strcmp(projects[f_idx].tasks[k].title, title) == 0) {
                                Task moved = projects[f_idx].tasks[k];
                                for (size_t j = k; j < projects[f_idx].task_count - 1; j++) {
                                    projects[f_idx].tasks[j] = projects[f_idx].tasks[j + 1];
                                }
                                projects[f_idx].task_count--;
                                if (projects[t_idx].task_count >= projects[t_idx].task_capacity) {
                                    projects[t_idx].task_capacity = projects[t_idx].task_capacity == 0 ? 4 : projects[t_idx].task_capacity * 2;
                                    Task *nxt = realloc(projects[t_idx].tasks, projects[t_idx].task_capacity * sizeof(Task));
                                    if (nxt) projects[t_idx].tasks = nxt;
                                }
                                projects[t_idx].tasks[projects[t_idx].task_count++] = moved;
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *args = line + 9;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *pname = args;
                char *title = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, pname) == 0) {
                        for (size_t k = 0; k < projects[i].task_count; k++) {
                            if (strcmp(projects[i].tasks[k].title, title) == 0) {
                                projects[i].tasks[k].done = 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *name = line + 14;
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
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("PROJECT %s\n", projects[i].name);
                for (size_t k = 0; k < projects[i].task_count; k++) {
                    printf("TASK %s %s %s\n", projects[i].tasks[k].title, projects[i].tasks[k].note, projects[i].tasks[k].done ? "DONE" : "TODO");
                }
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(projects[i].name);
        for (size_t k = 0; k < projects[i].task_count; k++) {
            free(projects[i].tasks[k].title);
            free(projects[i].tasks[k].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
