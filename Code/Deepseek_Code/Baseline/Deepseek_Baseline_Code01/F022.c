// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct task {
    char *title;
    char *note;
    int done;
    struct task *next;
} Task;

typedef struct project {
    char *name;
    Task *tasks;
    struct project *next;
} Project;

Project *projects = NULL;

Project *find_project(const char *name) {
    Project *p = projects;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

Task *find_task_in_project(Project *p, const char *title) {
    Task *t = p->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_task_to_project(Project *p, const char *title, const char *note) {
    if (find_task_in_project(p, title)) return;
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void remove_task_from_project(Project *p, const char *title) {
    Task *t = p->tasks, *prev = NULL;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else p->tasks = t->next;
            free(t->title);
            free(t->note);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void project_create(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void delete_project(const char *name) {
    Project *p = projects, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else projects = p->next;
            free(p->name);
            Task *t = p->tasks;
            while (t) { Task *tmp = t; t = t->next; free(tmp->title); free(tmp->note); free(tmp); }
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void move_task(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *t = find_task_in_project(pf, title);
    if (!t || find_task_in_project(pt, title)) return;
    remove_task_from_project(pf, title);
    add_task_to_project(pt, title, t->note);
    free(t->title);
    free(t->note);
    free(t);
}

void done_task_project(const char *proj, const char *title) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void report_projects(void) {
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                project_create(arg1);
        } else if (strcmp(cmd, "TASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %[^\n]", arg1, arg2, arg3) == 3) {
                Project *p = find_project(arg1);
                if (p) add_task_to_project(p, arg2, arg3);
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", arg1, arg2, arg3) == 3)
                move_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                done_task_project(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                delete_project(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }
    return 0;
}