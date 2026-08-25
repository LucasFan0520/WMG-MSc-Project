// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
} Project;

typedef struct {
    char *project;
    char *title;
    char *note;
    int done;
} Task;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Project *projects = NULL;
    int p_count = 0;
    Task *tasks = NULL;
    int t_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "REPORT") == 0) {
                for (int i = 0; i < p_count; i++) {
                    printf("Project: %s\n", projects[i].name);
                    for (int j = 0; j < t_count; j++) {
                        if (strcmp(tasks[j].project, projects[i].name) == 0) {
                            printf("  %s [%s]: %s\n", tasks[j].title, tasks[j].done ? "DONE" : "TODO", tasks[j].note);
                        }
                    }
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "PROJECT") == 0) {
            int found = -1;
            for (int i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, rest) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                projects = realloc(projects, (p_count + 1) * sizeof(Project));
                projects[p_count].name = strdup(rest);
                p_count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *pname = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *title = rest2;
                    char *note = p3 + 1;
                    int p_found = -1;
                    for (int i = 0; i < p_count; i++) {
                        if (strcmp(projects[i].name, pname) == 0) {
                            p_found = i;
                            break;
                        }
                    }
                    if (p_found != -1) {
                        tasks = realloc(tasks, (t_count + 1) * sizeof(Task));
                        tasks[t_count].project = strdup(pname);
                        tasks[t_count].title = strdup(title);
                        tasks[t_count].note = strdup(note);
                        tasks[t_count].done = 0;
                        t_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *fromP = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *toP = rest2;
                    char *title = p3 + 1;
                    int to_found = -1;
                    for (int i = 0; i < p_count; i++) {
                        if (strcmp(projects[i].name, toP) == 0) {
                            to_found = i;
                            break;
                        }
                    }
                    if (to_found != -1) {
                        for (int i = 0; i < t_count; i++) {
                            if (strcmp(tasks[i].project, fromP) == 0 && strcmp(tasks[i].title, title) == 0) {
                                free(tasks[i].project);
                                tasks[i].project = strdup(toP);
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *pname = rest;
                char *title = p2 + 1;
                for (int i = 0; i < t_count; i++) {
                    if (strcmp(tasks[i].project, pname) == 0 && strcmp(tasks[i].title, title) == 0) {
                        tasks[i].done = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *pname = rest;
            int p_found = -1;
            for (int i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, pname) == 0) {
                    p_found = i;
                    break;
                }
            }
            if (p_found != -1) {
                free(projects[p_found].name);
                for (int i = p_found; i < p_count - 1; i++) {
                    projects[i] = projects[i + 1];
                }
                p_count--;
                if (p_count == 0) {
                    free(projects);
                    projects = NULL;
                } else {
                    projects = realloc(projects, p_count * sizeof(Project));
                }
                int idx = 0;
                while (idx < t_count) {
                    if (strcmp(tasks[idx].project, pname) == 0) {
                        free(tasks[idx].project);
                        free(tasks[idx].title);
                        free(tasks[idx].note);
                        for (int j = idx; j < t_count - 1; j++) {
                            tasks[j] = tasks[j + 1];
                        }
                        t_count--;
                    } else {
                        idx++;
                    }
                }
                if (t_count == 0) {
                    free(tasks);
                    tasks = NULL;
                } else {
                    tasks = realloc(tasks, t_count * sizeof(Task));
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < p_count; i++) {
        free(projects[i].name);
    }
    free(projects);
    for (int i = 0; i < t_count; i++) {
        free(tasks[i].project);
        free(tasks[i].title);
        free(tasks[i].note);
    }
    free(tasks);
    return 0;
}