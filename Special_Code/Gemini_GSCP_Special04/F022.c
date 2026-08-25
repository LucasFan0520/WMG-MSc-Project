/* F022.c */
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
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
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

int main(void) {
    Project *projects = NULL;
    size_t p_count = 0;
    size_t p_cap = 0;
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
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (p_count >= p_cap) {
                    p_cap = p_cap == 0 ? 4 : p_cap * 2;
                    Project *np = realloc(projects, p_cap * sizeof(Project));
                    if (np) projects = np;
                }
                char *nn = mystrdup(name);
                if (nn) {
                    projects[p_count].name = nn;
                    projects[p_count].tasks = NULL;
                    projects[p_count].t_count = 0;
                    projects[p_count].t_cap = 0;
                    p_count++;
                }
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            while (*p == ' ') p++;
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            int found = -1;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                Project *pr = &projects[found];
                if (pr->t_count >= pr->t_cap) {
                    pr->t_cap = pr->t_cap == 0 ? 4 : pr->t_cap * 2;
                    Task *nt = realloc(pr->tasks, pr->t_cap * sizeof(Task));
                    if (nt) pr->tasks = nt;
                }
                char *ntitle = mystrdup(title);
                char *nnote = mystrdup(note);
                if (ntitle && nnote) {
                    pr->tasks[pr->t_count].title = ntitle;
                    pr->tasks[pr->t_count].note = nnote;
                    pr->tasks[pr->t_count].done = 0;
                    pr->t_count++;
                } else {
                    free(ntitle);
                    free(nnote);
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            while (*p == ' ') p++;
            char *fromP = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *toP = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int idxFrom = -1, idxTo = -1;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, fromP) == 0) idxFrom = (int)i;
                if (strcmp(projects[i].name, toP) == 0) idxTo = (int)i;
            }
            if (idxFrom != -1 && idxTo != -1) {
                Project *pf = &projects[idxFrom];
                Project *pt = &projects[idxTo];
                int t_idx = -1;
                for (size_t j = 0; j < pf->t_count; j++) {
                    if (strcmp(pf->tasks[j].title, title) == 0) {
                        t_idx = (int)j;
                        break;
                    }
                }
                if (t_idx != -1) {
                    if (pt->t_count >= pt->t_cap) {
                        pt->t_cap = pt->t_cap == 0 ? 4 : pt->t_cap * 2;
                        Task *nt = realloc(pt->tasks, pt->t_cap * sizeof(Task));
                        if (nt) pt->tasks = nt;
                    }
                    pt->tasks[pt->t_count] = pf->tasks[t_idx];
                    pt->t_count++;
                    for (size_t j = (size_t)t_idx; j < pf->t_count - 1; j++) {
                        pf->tasks[j] = pf->tasks[j + 1];
                    }
                    pf->t_count--;
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            while (*p == ' ') p++;
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, proj) == 0) {
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
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(projects[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(projects[found].name);
                for (size_t j = 0; j < projects[found].t_count; j++) {
                    free(projects[found].tasks[j].title);
                    free(projects[found].tasks[j].note);
                }
                free(projects[found].tasks);
                for (size_t i = (size_t)found; i < p_count - 1; i++) {
                    projects[i] = projects[i + 1];
                }
                p_count--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < p_count; i++) {
                printf("PROJECT: %s\n", projects[i].name);
                for (size_t j = 0; j < projects[i].t_count; j++) {
                    printf("  TASK: %s [%s] %s\n", projects[i].tasks[j].title, projects[i].tasks[j].done ? "DONE" : "TODO", projects[i].tasks[j].note);
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
