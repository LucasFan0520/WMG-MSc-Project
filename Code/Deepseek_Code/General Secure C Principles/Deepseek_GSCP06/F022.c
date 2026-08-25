// F022.c
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
    Task *t = find_task_in_project(pf, title);
    if (!t) return;
    if (find_task_in_project(pt, title)) return;
    // remove from pf
    Task *cur = pf->tasks, *prev = NULL;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else pf->tasks = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    // add to pt
    t->next = pt->tasks;
    pt->tasks = t;
}

void done_task(const char *proj_name, const char *title) {
    Project *p = find_project(proj_name);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *p = find_project(name);
    if (!p) return;
    Project *cur = projects, *prev = NULL;
    while (cur) {
        if (cur == p) {
            if (prev) prev->next = cur->next;
            else projects = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
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
}

void report_projects(void) {
    Project *p = projects;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("%s %s %s\n", t->title, t->done ? "done" : "pending", t->note);
            t = t->next;
        }
        p = p->next;
    }
}

void cleanup_projects(void) {
    Project *p = projects;
    while (p) {
        Project *tmp = p;
        p = p->next;
        free(tmp->name);
        Task *t = tmp->tasks;
        while (t) {
            Task *tt = t;
            t = t->next;
            free(tt->title);
            free(tt->note);
            free(tt);
        }
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%31s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (n < 2) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            add_project(arg1);
        } else if (strcmp(cmd, "TASK") == 0 && n >= 3) {
            char *note = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(arg2) + 1;
            add_task(arg1, arg2, note);
        } else if (strcmp(cmd, "MOVETASK") == 0 && n >= 4) {
            move_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0 && n >= 3) {
            done_task(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            delete_project(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }
    cleanup_projects();
    return 0;
}