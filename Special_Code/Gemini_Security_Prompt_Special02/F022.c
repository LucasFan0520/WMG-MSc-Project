// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *title;
    char *note;
    int done;
} Task;

typedef struct {
    char *name;
    Task *tasks;
    size_t t_count;
    size_t t_cap;
} Project;

int main() {
    Project *projects = NULL;
    size_t p_count = 0;
    size_t p_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *arg1 = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            if (p_count >= p_cap) {
                p_cap = p_cap == 0 ? 4 : p_cap * 2;
                projects = realloc(projects, p_cap * sizeof(Project));
            }
            projects[p_count].name = mystrdup(arg1);
            projects[p_count].tasks = NULL;
            projects[p_count].t_count = 0;
            projects[p_count].t_cap = 0;
            p_count++;
        } else if (strcmp(cmd, "TASK") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, arg1) == 0) {
                    if (projects[i].t_count >= projects[i].t_cap) {
                        projects[i].t_cap = projects[i].t_cap == 0 ? 4 : projects[i].t_cap * 2;
                        projects[i].tasks = realloc(projects[i].tasks, projects[i].t_cap * sizeof(Task));
                    }
                    projects[i].tasks[projects[i].t_count].title = mystrdup(title);
                    projects[i].tasks[projects[i].t_count].note = mystrdup(note);
                    projects[i].tasks[projects[i].t_count].done = 0;
                    projects[i].t_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            while (*p == ' ') p++;
            char *toProject = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            Project *src = NULL;
            Project *dst = NULL;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, arg1) == 0) src = &projects[i];
                if (strcmp(projects[i].name, toProject) == 0) dst = &projects[i];
            }
            if (src && dst) {
                for (size_t j = 0; j < src->t_count; j++) {
                    if (strcmp(src->tasks[j].title, title) == 0) {
                        if (dst->t_count >= dst->t_cap) {
                            dst->t_cap = dst->t_cap == 0 ? 4 : dst->t_cap * 2;
                            dst->tasks = realloc(dst->tasks, dst->t_cap * sizeof(Task));
                        }
                        dst->tasks[dst->t_count] = src->tasks[j];
                        dst->t_count++;
                        for (size_t k = j; k < src->t_count - 1; k++) {
                            src->tasks[k] = src->tasks[k + 1];
                        }
                        src->t_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, arg1) == 0) {
                    for (size_t j = 0; j < projects[i].t_count; j++) {
                        if (strcmp(projects[i].tasks[j].title, title) == 0) {
                            projects[i].tasks[j].done = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, arg1) == 0) {
                    free(projects[i].name);
                    for (size_t j = 0; j < projects[i].t_count; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    for (size_t j = i; j < p_count - 1; j++) {
                        projects[j] = projects[j + 1];
                    }
                    p_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < p_count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (size_t j = 0; j < projects[i].t_count; j++) {
                    printf("  Task: %s [%s] %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < p_count; i++) {
        free(projects[i].name);
        for (size_t j = 0; j < projects[i].t_count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
