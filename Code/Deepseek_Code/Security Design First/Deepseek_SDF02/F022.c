/* F022.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct task {
    char *title;
    char *note;
    int done;
    struct task *next;
} task_t;

typedef struct project {
    char *name;
    task_t *tasks;
    struct project *next;
} project_t;

project_t *projects = NULL;

project_t *find_project(const char *name) {
    project_t *p = projects;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

task_t *find_task_in_project(project_t *p, const char *title) {
    task_t *t = p->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void project_cmd(const char *name) {
    if (find_project(name)) return;
    project_t *p = malloc(sizeof(project_t));
    if (!p) return;
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
    if (!p->name) { free(p); }
}

void task_cmd(const char *pname, const char *title, const char *note) {
    project_t *p = find_project(pname);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    task_t *t = malloc(sizeof(task_t));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
    if (!t->title || !t->note) {
        free(t->title); free(t->note); free(t);
    }
}

void movetask_cmd(const char *from, const char *to, const char *title) {
    project_t *fp = find_project(from);
    project_t *tp = find_project(to);
    if (!fp || !tp) return;
    task_t *prev = NULL, *t = fp->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else fp->tasks = t->next;
            t->next = tp->tasks;
            tp->tasks = t;
            return;
        }
        prev = t;
        t = t->next;
    }
}

void donetask_cmd(const char *pname, const char *title) {
    project_t *p = find_project(pname);
    if (!p) return;
    task_t *t = find_task_in_project(p, title);
    if (!t) return;
    t->done = 1;
}

void deleteproject_cmd(const char *name) {
    project_t *prev = NULL, *p = projects;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else projects = p->next;
            free(p->name);
            task_t *t = p->tasks;
            while (t) {
                task_t *nxt = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = nxt;
            }
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void report_cmd(void) {
    project_t *p = projects;
    while (p) {
        printf("%s\n", p->name);
        task_t *t = p->tasks;
        while (t) {
            printf("%s %s %s\n", t->title, t->note, t->done ? "DONE" : "TODO");
            t = t->next;
        }
        p = p->next;
    }
}

void free_all(void) {
    project_t *p = projects;
    while (p) {
        project_t *nxt = p->next;
        free(p->name);
        task_t *t = p->tasks;
        while (t) {
            task_t *n = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = n;
        }
        free(p);
        p = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[256], b[256], c[256];
        if (sscanf(line, "%19s %255s %255s %255s", cmd, a, b, c) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "PROJECT") == 0) {
                project_cmd(a);
            } else if (strcmp(cmd, "TASK") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2) {
                    const char *note = line + strlen(cmd) + 1 + strlen(a) + 1 + strlen(b) + 1;
                    task_cmd(a, b, note);
                }
            } else if (strcmp(cmd, "MOVETASK") == 0) {
                if (sscanf(line, "%*s %255s %255s %255s", a, b, c) == 3)
                    movetask_cmd(a, b, c);
            } else if (strcmp(cmd, "DONETASK") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    donetask_cmd(a, b);
            } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
                deleteproject_cmd(a);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_cmd();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}