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
    Task *prev = NULL;
    Task *t = fp->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else fp->tasks = t->next;
            t->next = tp->tasks;
            tp->tasks = t;
            return;
        }
        prev = t;
        t = t->next;
    }
}

void done_task(const char *proj, const char *title) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *prev = NULL;
    Project *p = projects;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else projects = p->next;
            Task *t = p->tasks;
            while (t) {
                Task *next = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = next;
            }
            free(p->name);
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
            printf("  %s %s:", t->done ? "DONE" : "TODO", t->title);
            for (char *s = t->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
            putchar('\n');
            t = t->next;
        }
        p = p->next;
    }
}

void free_all() {
    Project *p = projects;
    while (p) {
        Project *nextp = p->next;
        Task *t = p->tasks;
        while (t) {
            Task *nextt = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = nextt;
        }
        free(p->name);
        free(p);
        p = nextp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "PROJECT") == 0) {
            create_project(rest);
        } else if (strcmp(cmd, "TASK") == 0) {
            char proj[256], title[256], note[4096];
            if (sscanf(rest, "%255s %255s %[^\n]", proj, title, note) == 3) {
                add_task(proj, title, note);
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256], to[256], title[256];
            if (sscanf(rest, "%255s %255s %255s", from, to, title) == 3) {
                move_task(from, to, title);
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char proj[256], title[256];
            if (sscanf(rest, "%255s %255s", proj, title) == 2) {
                done_task(proj, title);
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            delete_project(rest);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }
    free_all();
    return 0;
}