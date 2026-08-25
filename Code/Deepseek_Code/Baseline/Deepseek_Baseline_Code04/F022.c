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

void add_task(const char *proj, const char *title, const char *note) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *fp = find_project(from);
    Project *tp = find_project(to);
    if (!fp || !tp) return;
    Task *prev = NULL, *cur = fp->tasks;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else fp->tasks = cur->next;
            cur->next = tp->tasks;
            tp->tasks = cur;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void done_task(const char *proj, const char *title) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *prev = NULL, *cur = projects;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else projects = cur->next;
            free(cur->name);
            Task *t = cur->tasks;
            while (t) {
                Task *tmp = t;
                t = t->next;
                free(tmp->title);
                free(tmp->note);
                free(tmp);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void report(void) {
    Project *p = projects;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("%s %s\n", t->title, t->note);
            t = t->next;
        }
        p = p->next;
    }
}

void free_projects(void) {
    while (projects) {
        Project *tmp = projects;
        projects = projects->next;
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256], title[256], from[256], to[256];
        int n = sscanf(line, "%s %s %s %s", cmd, name, from, to);
        if (strcmp(cmd, "PROJECT") == 0) {
            if (n < 2) continue;
            add_project(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            if (n < 3) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1 + strlen(from) + 1;
            while (*p == ' ') p++;
            if (*p) add_task(name, from, p);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            if (n < 4) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1 + strlen(from) + 1 + strlen(to) + 1;
            while (*p == ' ') p++;
            if (*p) move_task(name, from, p);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            if (n < 3) continue;
            done_task(name, from);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            if (n < 2) continue;
            delete_project(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_projects();
    return 0;
}