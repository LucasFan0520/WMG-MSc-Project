// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

typedef struct {
    char *title;
    char *note;
    int done;
} Task;

typedef struct {
    char *name;
    Task *tasks;
    int task_count;
    int task_cap;
} Project;

int main() {
    Project *projects = NULL;
    int p_count = 0;
    int p_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
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
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < p_count; i++) {
                printf("PROJECT %s\n", projects[i].name);
                for (int j = 0; j < projects[i].task_count; j++) {
                    printf("%s %s [%s]\n", projects[i].tasks[j].title, projects[i].tasks[j].note, projects[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (int i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (p_count >= p_cap) {
                    p_cap = p_cap == 0 ? 16 : p_cap * 2;
                    Project *nb = realloc(projects, p_cap * sizeof(Project));
                    if (nb) projects = nb;
                }
                projects[p_count].name = strdup(name);
                projects[p_count].tasks = NULL;
                projects[p_count].task_count = 0;
                projects[p_count].task_cap = 0;
                p_count++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            for (int i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
                    Project *pr = &projects[i];
                    if (pr->task_count >= pr->task_cap) {
                        pr->task_cap = pr->task_cap == 0 ? 16 : pr->task_cap * 2;
                        Task *nb = realloc(pr->tasks, pr->task_cap * sizeof(Task));
                        if (nb) pr->tasks = nb;
                    }
                    pr->tasks[pr->task_count].title = strdup(title);
                    pr->tasks[pr->task_count].note = strdup(note);
                    pr->tasks[pr->task_count].done = 0;
                    pr->task_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *fromProj = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *toProj = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int f_idx = -1, t_idx = -1;
            for (int i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, fromProj) == 0) f_idx = i;
                if (strcmp(projects[i].name, toProj) == 0) t_idx = i;
            }
            if (f_idx != -1 && t_idx != -1) {
                Project *fp = &projects[f_idx];
                Project *tp = &projects[t_idx];
                for (int j = 0; j < fp->task_count; j++) {
                    if (strcmp(fp->tasks[j].title, title) == 0) {
                        if (tp->task_count >= tp->task_cap) {
                            tp->task_cap = tp->task_cap == 0 ? 16 : tp->task_cap * 2;
                            Task *nb = realloc(tp->tasks, tp->task_cap * sizeof(Task));
                            if (nb) tp->tasks = nb;
                        }
                        tp->tasks[tp->task_count] = fp->tasks[j];
                        tp->task_count++;
                        for (int k = j; k < fp->task_count - 1; k++) {
                            fp->tasks[k] = fp->tasks[k + 1];
                        }
                        fp->task_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < p_count; i++) {
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
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    free(projects[i].name);
                    for (int j = 0; j < projects[i].task_count; j++) {
                        free(projects[i].tasks[j].title);
                        free(projects[i].tasks[j].note);
                    }
                    free(projects[i].tasks);
                    for (int j = i; j < p_count - 1; j++) {
                        projects[j] = projects[j + 1];
                    }
                    p_count--;
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < p_count; i++) {
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
