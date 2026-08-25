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
    for (Project *p = projects; p; p = p->next) {
        if (strcmp(p->name, name) == 0) return p;
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

void add_task(const char *pname, const char *title, const char *note) {
    Project *p = find_project(pname);
    if (!p) return;
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
    Task **tp = &pf->tasks;
    while (*tp) {
        if (strcmp((*tp)->title, title) == 0) {
            Task *tmp = *tp;
            *tp = tmp->next;
            tmp->next = pt->tasks;
            pt->tasks = tmp;
            return;
        }
        tp = &(*tp)->next;
    }
}

void done_task(const char *pname, const char *title) {
    Project *p = find_project(pname);
    if (!p) return;
    for (Task *t = p->tasks; t; t = t->next) {
        if (strcmp(t->title, title) == 0) {
            t->done = 1;
            return;
        }
    }
}

void delete_project(const char *name) {
    Project **pp = &projects;
    while (*pp) {
        if (strcmp((*pp)->name, name) == 0) {
            Project *tmp = *pp;
            *pp = tmp->next;
            Task *t = tmp->tasks;
            while (t) {
                Task *next_t = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = next_t;
            }
            free(tmp->name);
            free(tmp);
            return;
        }
        pp = &(*pp)->next;
    }
}

void report() {
    for (Project *p = projects; p; p = p->next) {
        printf("%s\n", p->name);
        for (Task *t = p->tasks; t; t = t->next) {
            printf("%s %s %s\n", t->title, t->done ? "DONE" : "TODO", t->note);
        }
    }
}

void cleanup() {
    while (projects) {
        Project *tmp = projects->next;
        Task *t = projects->tasks;
        while (t) {
            Task *next_t = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = next_t;
        }
        free(projects->name);
        free(projects);
        projects = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) add_project(a1);
        } else if (strcmp(cmd, "TASK") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) add_task(a1, a2, t + 1); } }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", a1, a2, a3) == 3) move_task(a1, a2, a3);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) done_task(a1, a2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_project(a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    cleanup();
    return 0;
}