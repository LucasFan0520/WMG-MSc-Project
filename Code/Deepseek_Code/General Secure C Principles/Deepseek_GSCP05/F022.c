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

void create_project(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) return;
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void add_task(const char *proj, const char *title, const char *note) {
    Project *p = find_project(proj);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
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
            // add to target
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
            Task *t = cur->tasks;
            while (t) {
                Task *next = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = next;
            }
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void report_projects(void) {
    Project *p = projects;
    while (p) {
        printf("%s:\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("%s %s ", t->done ? "[DONE]" : "[TODO]", t->title);
            char *s = t->note;
            for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
            putchar('\n');
            t = t->next;
        }
        p = p->next;
    }
}

void free_all(void) {
    Project *p = projects;
    while (p) {
        Project *next = p->next;
        Task *t = p->tasks;
        while (t) {
            Task *tnext = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = tnext;
        }
        free(p->name);
        free(p);
        p = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            create_project(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *proj = p;
            char *space = strchr(proj, ' ');
            if (!space) continue;
            *space = '\0';
            char *title = space + 1;
            while (*title == ' ') title++;
            space = strchr(title, ' ');
            if (!space) continue;
            *space = '\0';
            char *note = space + 1;
            while (*note == ' ') note++;
            add_task(proj, title, note);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *p = line + 10;
            while (*p == ' ') p++;
            char *from = p;
            char *space = strchr(from, ' ');
            if (!space) continue;
            *space = '\0';
            char *to = space + 1;
            while (*to == ' ') to++;
            space = strchr(to, ' ');
            if (!space) continue;
            *space = '\0';
            char *title = space + 1;
            while (*title == ' ') title++;
            move_task(from, to, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *p = line + 10;
            while (*p == ' ') p++;
            char *proj = p;
            char *space = strchr(proj, ' ');
            if (!space) continue;
            *space = '\0';
            char *title = space + 1;
            while (*title == ' ') title++;
            done_task(proj, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *p = line + 14;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            delete_project(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }
    free_all();
    return 0;
}