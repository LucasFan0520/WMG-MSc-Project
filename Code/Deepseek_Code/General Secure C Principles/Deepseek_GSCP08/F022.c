// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task { char *title; char *note; struct Task *next; } Task;
typedef struct Project { char *name; Task *tasks; struct Project *next; } Project;
Project *projects = NULL;

Project *find_project(const char *name) {
    for (Project *p = projects; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

Task *find_task_in_project(Project *p, const char *title) {
    for (Task *t = p->tasks; t; t = t->next)
        if (strcmp(t->title, title) == 0) return t;
    return NULL;
}

void project_create(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) return;
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void task_add(const char *pname, const char *title, const char *note) {
    Project *p = find_project(pname);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from), *pt = find_project(to);
    if (!pf || !pt) return;
    Task *prev = NULL;
    for (Task *t = pf->tasks; t; prev = t, t = t->next) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next; else pf->tasks = t->next;
            t->next = pt->tasks;
            pt->tasks = t;
            return;
        }
    }
}

void done_task(const char *pname, const char *title) {
    // mark as done: remove from project? The spec says "DONETASK project title marks it done"
    // No explicit done list; we can just remove it or leave it. We'll remove it.
    Project *p = find_project(pname);
    if (!p) return;
    Task *prev = NULL;
    for (Task *t = p->tasks; t; prev = t, t = t->next) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next; else p->tasks = t->next;
            free(t->title); free(t->note); free(t);
            return;
        }
    }
}

void delete_project(const char *name) {
    Project *prev = NULL;
    for (Project *p = projects; p; prev = p, p = p->next) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next; else projects = p->next;
            free(p->name);
            while (p->tasks) { Task *t = p->tasks; p->tasks = t->next; free(t->title); free(t->note); free(t); }
            free(p);
            return;
        }
    }
}

void report_projects(void) {
    for (Project *p = projects; p; p = p->next) {
        printf("%s\n", p->name);
        for (Task *t = p->tasks; t; t = t->next)
            printf("%s %s\n", t->title, t->note);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "PROJECT") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) project_create(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            char pname[256], title[256], note[4096];
            if (sscanf(rest, "%255s %255s", pname, title) == 2) {
                char *n = rest + strlen(pname) + 1 + strlen(title);
                while (*n == ' ') n++;
                strcpy(note, n);
                task_add(pname, title, note);
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256], to[256], title[256];
            if (sscanf(rest, "%255s %255s %255s", from, to, title) == 3)
                move_task(from, to, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char pname[256], title[256];
            if (sscanf(rest, "%255s %255s", pname, title) == 2)
                done_task(pname, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_project(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }
    while (projects) { Project *p = projects; projects = p->next; free(p->name); while (p->tasks) { Task *t = p->tasks; p->tasks = t->next; free(t->title); free(t->note); free(t); } free(p); }
    return 0;
}