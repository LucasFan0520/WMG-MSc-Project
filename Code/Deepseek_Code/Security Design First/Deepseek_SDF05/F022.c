// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

Task *find_task(Project *p, const char *title) {
    Task *t = p->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void project_create(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) return;
    p->name = safe_strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void task_add(const char *project_name, const char *title, const char *note) {
    Project *p = find_project(project_name);
    if (!p) return;
    if (find_task(p, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = safe_strdup(title);
    t->note = safe_strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void task_move(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *t = find_task(pf, title);
    if (!t) return;
    // Remove from pf
    Task *prev = NULL, *cur = pf->tasks;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else pf->tasks = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    // Add to pt
    t->next = pt->tasks;
    pt->tasks = t;
}

void task_done(const char *project_name, const char *title) {
    Project *p = find_project(project_name);
    if (!p) return;
    Task *t = find_task(p, title);
    if (t) t->done = 1;
}

void project_delete(const char *name) {
    Project *prev = NULL, *cur = projects;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else projects = cur->next;
            free(cur->name);
            Task *t = cur->tasks;
            while (t) {
                Task *next = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void project_report(void) {
    Project *p = projects;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("%s %s %s\n", t->title, t->note, t->done ? "DONE" : "TODO");
            t = t->next;
        }
        p = p->next;
    }
}

void free_all(void) {
    Project *p = projects;
    while (p) {
        Project *next_p = p->next;
        free(p->name);
        Task *t = p->tasks;
        while (t) {
            Task *next_t = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = next_t;
        }
        free(p);
        p = next_p;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512], arg3[512];
        int n = sscanf(line, "%31s %511s %511s %511s", cmd, arg1, arg2, arg3);
        if (n < 1) continue;

        if (strcmp(cmd, "PROJECT") == 0 && n >= 2) {
            project_create(arg1);
        } else if (strcmp(cmd, "TASK") == 0 && n >= 3) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            task_add(arg1, arg2, p);
        } else if (strcmp(cmd, "MOVETASK") == 0 && n >= 4) {
            task_move(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0 && n >= 3) {
            task_done(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0 && n >= 2) {
            project_delete(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            project_report();
        }
    }
    free_all();
    return 0;
}