/* F022.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    int done;
    struct Task *next;
} Task;

typedef struct Project {
    char *name;
    Task *tasks;
    struct Project *next;
} Project;

Project *projects = NULL;

Project* find_project(const char *name) {
    Project *p = projects;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

Task* find_task_in_project(Project *p, const char *title) {
    Task *t = p->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_project(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void add_task(const char *proj_name, const char *title, const char *note) {
    Project *p = find_project(proj_name);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *t = pf->tasks, *prev = NULL;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else pf->tasks = t->next;
            t->next = pt->tasks;
            pt->tasks = t;
            return;
        }
        prev = t;
        t = t->next;
    }
}

void done_task(const char *proj_name, const char *title) {
    Project *p = find_project(proj_name);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *p = projects, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else projects = p->next;
            free(p->name);
            Task *t = p->tasks;
            while (t) {
                Task *tmp = t;
                t = t->next;
                free(tmp->title);
                free(tmp->note);
                free(tmp);
            }
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void report_projects() {
    Project *p = projects;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("%s %s %s\n", t->title, t->note, t->done ? "done" : "pending");
            t = t->next;
        }
        p = p->next;
    }
}

void free_all() {
    while (projects) {
        Project *tmp = projects;
        projects = projects->next;
        free(tmp->name);
        Task *t = tmp->tasks;
        while (t) {
            Task *ttmp = t;
            t = t->next;
            free(ttmp->title);
            free(ttmp->note);
            free(ttmp);
        }
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[256], c[256], d[4096];
        int n = sscanf(line, "%31s %255s %255s %255s %[^\n]", cmd, a, b, c, d);
        if (strcmp(cmd, "PROJECT") == 0 && n >= 2) {
            add_project(a);
        } else if (strcmp(cmd, "TASK") == 0 && n >= 4) {
            add_task(a, b, c);
        } else if (strcmp(cmd, "MOVETASK") == 0 && n >= 4) {
            move_task(a, b, c);
        } else if (strcmp(cmd, "DONETASK") == 0 && n >= 3) {
            done_task(a, b);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0 && n >= 2) {
            delete_project(a);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_projects();
        }
    }
    free_all();
    return 0;
}