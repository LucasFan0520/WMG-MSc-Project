/*
 * F022.c
 */
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

Task *find_task(Project *p, const char *title) {
    Task *t = p->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void project_create(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void project_add_task(const char *pname, const char *title, const char *note) {
    Project *p = find_project(pname);
    if (!p) return;
    if (find_task(p, title)) return;
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void project_move_task(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *prev = NULL, *t = pf->tasks;
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

void project_done_task(const char *pname, const char *title) {
    Project *p = find_project(pname);
    if (!p) return;
    Task *t = find_task(p, title);
    if (t) t->done = 1;
}

void project_delete(const char *name) {
    Project *prev = NULL, *p = projects;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else projects = p->next;
            Task *t = p->tasks;
            while (t) {
                Task *tmp = t;
                t = t->next;
                free(tmp->title);
                free(tmp->note);
                free(tmp);
            }
            free(p->name);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void project_report(void) {
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
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[256];
        if (sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "PROJECT") == 0) {
            project_create(arg1);
        } else if (strcmp(cmd, "TASK") == 0) {
            project_add_task(arg1, arg2, rest + strlen(arg2) + 1);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            project_move_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            project_done_task(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            project_delete(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            project_report();
        }
    }
    return 0;
}