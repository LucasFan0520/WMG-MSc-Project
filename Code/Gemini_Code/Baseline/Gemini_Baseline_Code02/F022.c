// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

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
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("Project: %s\n", projects[i].name);
                    for (int j = 0; j < projects[i].task_count; j++) {
                        printf("  Task: %s [%s] - %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                    }
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "PROJECT") == 0) {
            projects = realloc(projects, sizeof(Project) * (count + 1));
            projects[count].name = strdup(args);
            projects[count].tasks = NULL;
            projects[count].task_count = 0;
            count++;
        } else if (strcmp(cmd, "TASK") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *proj = args;
                char *title = p2 + 1;
                char *p3 = strchr(title, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *note = p3 + 1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, proj) == 0) {
                            int tc = projects[i].task_count;
                            projects[i].tasks = realloc(projects[i].tasks, sizeof(Task) * (tc + 1));
                            projects[i].tasks[tc].title = strdup(title);
                            projects[i].tasks[tc].note = strdup(note);
                            projects[i].tasks[tc].done = 0;
                            projects[i].task_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *fromProj = args;
            char *p2 = strchr(fromProj, ' ');
            if (p2) {
                *p2 = '\0';
                char *toProj = p2 + 1;
                char *p3 = strchr(toProj, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *title = p3 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(projects[i].name, fromProj) == 0) f_idx = i;
                        if (strcmp(projects[i].name, toProj) == 0) t_idx = i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        for (int j = 0; j < projects[f_idx].task_count; j++) {
                            if (strcmp(projects[f_idx].tasks[j].title, title) == 0) {
                                int tc = projects[t_idx].task_count;
                                projects[t_idx].tasks = realloc(projects[t_idx].tasks, sizeof(Task) * (tc + 1));
                                projects[t_idx].tasks[tc] = projects[f_idx].tasks[j];
                                projects[t_idx].task_count++;
                                for (int k = j; k < projects[f_idx].task_count - 1; k++) {
                                    projects[f_idx].tasks[k] = projects[f_idx].tasks[k + 1];
                                }
                                projects[f_idx].task_count--;
                                if (projects[f_idx].task_count == 0) {
                                    free(projects[f_idx].tasks);
                                    projects[f_idx].tasks = NULL;
                                } else {
                                    projects[f_idx].tasks = realloc(projects[f_idx].tasks, sizeof(Task) * projects[f_idx].task_count);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *proj = args;
                char *title = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, proj) == 0) {
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
                    if (count == 0) { free(projects); projects = NULL; }
                    else { projects = realloc(projects, sizeof(Project) * count); }
                    break;
                }
            }
        }
        free(line);
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
