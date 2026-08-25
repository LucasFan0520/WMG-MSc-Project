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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
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
    int count;
    int cap;
} Project;

int main(void) {
    Project *projects = NULL;
    int p_count = 0;
    int p_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                if (p_count >= p_cap) {
                    p_cap = p_cap == 0 ? 16 : p_cap * 2;
                    projects = realloc(projects, p_cap * sizeof(Project));
                }
                projects[p_count].name = strdup(name);
                projects[p_count].tasks = NULL;
                projects[p_count].count = 0;
                projects[p_count].cap = 0;
                p_count++;
            }
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *title = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    while (*note == ' ') note++;
                    for (int i = 0; i < p_count; i++) {
                        if (strcmp(projects[i].name, proj) == 0) {
                            Project *pr = &projects[i];
                            if (pr->count >= pr->cap) {
                                pr->cap = pr->cap == 0 ? 16 : pr->cap * 2;
                                pr->tasks = realloc(pr->tasks, pr->cap * sizeof(Task));
                            }
                            pr->tasks[pr->count].title = strdup(title);
                            pr->tasks[pr->count].note = strdup(note);
                            pr->tasks[pr->count].done = 0;
                            pr->count++;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *fromP = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *toP = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *title = p + 1;
                    while (*title == ' ') title++;
                    int f_idx = -1, t_idx = -1;
                    for (int i = 0; i < p_count; i++) {
                        if (strcmp(projects[i].name, fromP) == 0) f_idx = i;
                        if (strcmp(projects[i].name, toP) == 0) t_idx = i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        int t_match = -1;
                        Project *fp = &projects[f_idx];
                        for (int j = 0; j < fp->count; j++) {
                            if (strcmp(fp->tasks[j].title, title) == 0) {
                                t_match = j;
                                break;
                            }
                        }
                        if (t_match != -1) {
                            Project *tp = &projects[t_idx];
                            if (tp->count >= tp->cap) {
                                tp->cap = tp->cap == 0 ? 16 : tp->cap * 2;
                                tp->tasks = realloc(tp->tasks, tp->cap * sizeof(Task));
                            }
                            tp->tasks[tp->count] = fp->tasks[t_match];
                            tp->count++;
                            for (int j = t_match; j < fp->count - 1; j++) {
                                fp->tasks[j] = fp->tasks[j + 1];
                            }
                            fp->count--;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *title = p + 1;
                while (*title == ' ') title++;
                for (int i = 0; i < p_count; i++) {
                    if (strcmp(projects[i].name, proj) == 0) {
                        for (int j = 0; j < projects[i].count; j++) {
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
            while (*name == ' ') name++;
            int idx = -1;
            for (int i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(projects[idx].name);
                for (int j = 0; j < projects[idx].count; j++) {
                    free(projects[idx].tasks[j].title);
                    free(projects[idx].tasks[j].note);
                }
                free(projects[idx].tasks);
                for (int i = idx; i < p_count - 1; i++) {
                    projects[i] = projects[i + 1];
                }
                p_count--;
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < p_count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (int j = 0; j < projects[i].count; j++) {
                    printf("  Task: %s [%s] - %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < p_count; i++) {
        free(projects[i].name);
        for (int j = 0; j < projects[i].count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
