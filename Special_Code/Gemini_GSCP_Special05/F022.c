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
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
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
    size_t count;
    size_t cap;
} Project;

int main(void) {
    Project *projects = NULL;
    size_t p_count = 0;
    size_t p_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PROJECT ", 8) == 0) {
            char *name = line + 8;
            if (p_count >= p_cap) {
                p_cap = p_cap == 0 ? 4 : p_cap * 2;
                Project *nb = realloc(projects, p_cap * sizeof(Project));
                if (!nb) {
                    free(line);
                    break;
                }
                projects = nb;
            }
            projects[p_count].name = safe_dup(name);
            projects[p_count].tasks = NULL;
            projects[p_count].count = 0;
            projects[p_count].cap = 0;
            p_count++;
        } else if (strncmp(line, "TASK ", 5) == 0) {
            char *p = line + 5;
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *title = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
                    Project *pr = &projects[i];
                    if (pr->count >= pr->cap) {
                        pr->cap = pr->cap == 0 ? 4 : pr->cap * 2;
                        Task *nb = realloc(pr->tasks, pr->cap * sizeof(Task));
                        if (!nb) break;
                        pr->tasks = nb;
                    }
                    pr->tasks[pr->count].title = safe_dup(title);
                    pr->tasks[pr->count].note = safe_dup(note);
                    pr->tasks[pr->count].done = 0;
                    pr->count++;
                    break;
                }
            }
        } else if (strncmp(line, "MOVETASK ", 9) == 0) {
            char *p = line + 9;
            char *from = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *to = p;
            while (*p && *p != ' ') p++;
            char *title = "";
            if (*p == ' ') {
                *p = '\0';
                title = p + 1;
            }
            Project *p_from = NULL, *p_to = NULL;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, from) == 0) p_from = &projects[i];
                if (strcmp(projects[i].name, to) == 0) p_to = &projects[i];
            }
            if (p_from && p_to) {
                for (size_t j = 0; j < p_from->count; j++) {
                    if (strcmp(p_from->tasks[j].title, title) == 0) {
                        if (p_to->count >= p_to->cap) {
                            p_to->cap = p_to->cap == 0 ? 4 : p_to->cap * 2;
                            Task *nb = realloc(p_to->tasks, p_to->cap * sizeof(Task));
                            if (!nb) break;
                            p_to->tasks = nb;
                        }
                        p_to->tasks[p_to->count] = p_from->tasks[j];
                        p_to->count++;
                        for (size_t k = j; k < p_from->count - 1; k++) {
                            p_from->tasks[k] = p_from->tasks[k + 1];
                        }
                        p_from->count--;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DONETASK ", 9) == 0) {
            char *p = line + 9;
            char *proj = p;
            while (*p && *p != ' ') p++;
            char *title = "";
            if (*p == ' ') {
                *p = '\0';
                title = p + 1;
            }
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
                    for (size_t j = 0; j < projects[i].count; j++) {
                        if (strcmp(projects[i].tasks[j].title, title) == 0) {
                            projects[i].tasks[j].done = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETEPROJECT ", 14) == 0) {
            char *name = line + 14;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    free(projects[i].name);
                    for (size_t j = 0; j < projects[i].count; j++) {
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
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < p_count; i++) {
                printf("Project: %s\n", projects[i].name);
                for (size_t j = 0; j < projects[i].count; j++) {
                    printf("  Task: %s [%s] - %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < p_count; i++) {
        free(projects[i].name);
        for (size_t j = 0; j < projects[i].count; j++) {
            free(projects[i].tasks[j].title);
            free(projects[i].tasks[j].note);
        }
        free(projects[i].tasks);
    }
    free(projects);
    return 0;
}
