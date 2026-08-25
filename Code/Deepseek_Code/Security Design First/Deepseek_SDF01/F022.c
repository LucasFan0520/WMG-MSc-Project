// filename: F022.c
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

static Project *projects = NULL;

static void free_tasks(Task *t) {
    while (t) {
        Task *tmp = t;
        t = t->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
}

static void cleanup(void) {
    while (projects) {
        Project *tmp = projects;
        projects = projects->next;
        free(tmp->name);
        free_tasks(tmp->tasks);
        free(tmp);
    }
}

static Project *find_project(const char *name) {
    for (Project *p = projects; p; p = p->next)
        if (strcmp(p->name, name) == 0)
            return p;
    return NULL;
}

static Task *find_task_in_project(Project *p, const char *title) {
    for (Task *t = p->tasks; t; t = t->next)
        if (strcmp(t->title, title) == 0)
            return t;
    return NULL;
}

static Task *find_task_anywhere(const char *title, Project **out_proj) {
    for (Project *p = projects; p; p = p->next) {
        Task *t = find_task_in_project(p, title);
        if (t) {
            *out_proj = p;
            return t;
        }
    }
    return NULL;
}

static void project_create(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) return;
    p->name = strdup(name);
    if (!p->name) { free(p); return; }
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

static void task_add(const char *proj_name, const char *title, const char *note) {
    Project *p = find_project(proj_name);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    if (!t->title || !t->note) {
        free(t->title);
        free(t->note);
        free(t);
        return;
    }
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

static void task_move(const char *from, const char *to, const char *title) {
    Project *fp = find_project(from);
    Project *tp = find_project(to);
    if (!fp || !tp) return;
    Task *prev = NULL;
    Task *t = NULL;
    for (Task *cur = fp->tasks; cur; cur = cur->next) {
        if (strcmp(cur->title, title) == 0) {
            t = cur;
            break;
        }
        prev = cur;
    }
    if (!t) return;
    if (prev) prev->next = t->next;
    else fp->tasks = t->next;
    t->next = tp->tasks;
    tp->tasks = t;
}

static void task_done(const char *proj_name, const char *title) {
    Project *p = find_project(proj_name);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

static void project_delete(const char *name) {
    Project *prev = NULL;
    for (Project *p = projects; p; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else projects = p->next;
            free(p->name);
            free_tasks(p->tasks);
            free(p);
            return;
        }
        prev = p;
    }
}

static void report(void) {
    for (Project *p = projects; p; p = p->next) {
        printf("%s:\n", p->name);
        for (Task *t = p->tasks; t; t = t->next)
            printf("  %s %s %s\n", t->title, t->note, t->done ? "(done)" : "");
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "PROJECT") == 0) {
            if (*arg1) project_create(arg1);
        } else if (strcmp(cmd, "TASK") == 0) {
            if (*arg1 && *arg2) task_add(arg1, arg2, arg3);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            if (*arg1 && *arg2 && *arg3) task_move(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            if (*arg1 && *arg2) task_done(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            if (*arg1) project_delete(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free(line);
    return 0;
}