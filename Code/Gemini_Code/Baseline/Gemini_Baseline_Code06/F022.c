// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
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
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            projects = realloc(projects, (count + 1) * sizeof(Project));
            projects[count].name = strdup(name);
            projects[count].tasks = NULL;
            projects[count].task_count = 0;
            count++;
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char proj[128], title[128];
            if (sscanf(line + 5, "%127s %127s", proj, title) == 2) {
                char *p = line + 5;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, proj) == 0) {
                        projects[i].tasks = realloc(projects[i].tasks, (projects[i].task_count + 1) * sizeof(Task));
                        projects[i].tasks[projects[i].task_count].title = strdup(title);
                        projects[i].tasks[projects[i].task_count].note = strdup(p);
                        projects[i].tasks[projects[i].task_count].done = 0;
                        projects[i].task_count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char fromP[128], toP[128], title[128];
            if (sscanf(line + 9, "%127s %127s %127s", fromP, toP, title) == 3) {
                int f_idx = -1, t_idx = -1, t_sub = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(projects[i].name, fromP) == 0) f_idx = i;
                    if (strcmp(projects[i].name, toP) == 0) t_idx = i;
                }
                if (f_idx != -1 && t_idx != -1) {
                    for (int j = 0; j < projects[f_idx].task_count; j++) {
                        if (strcmp(projects[f_idx].tasks[j].title, title) == 0) {
                            t_sub = j;
                            break;
                        }
                    }
                    if (t_sub != -1) {
                        projects[t_idx].tasks = realloc(projects[t_idx].tasks, (projects[t_idx].task_count + 1) * sizeof(Task));
                        projects[t_idx].tasks[projects[t_idx].task_count] = projects[f_idx].tasks[t_sub];
                        projects[t_idx].task_count++;
                        for (int j = t_sub; j < projects[f_idx].task_count - 1; j++) {
                            projects[f_idx].tasks[j] = projects[f_idx].tasks[j + 1];
                        }
                        projects[f_idx].task_count--;
                        projects[f_idx].tasks = realloc(projects[f_idx].tasks, projects[f_idx].task_count * sizeof(Task));
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char proj[128], title[128];
            if (sscanf(line + 9, "%127s %127s", proj, title) == 2) {
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
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *name = line + 14;
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
                    projects = realloc(projects, count * sizeof(Project));
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("  Task: %s [%s] %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
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
