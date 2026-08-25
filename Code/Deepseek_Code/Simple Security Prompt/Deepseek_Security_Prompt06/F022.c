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
    p->name = malloc(strlen(name) + 1);
    strcpy(p->name, name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void add_task(const char *pname, const char *title, const char *note) {
    Project *p = find_project(pname);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    Task *t = malloc(sizeof(Task));
    t->title = malloc(strlen(title) + 1);
    strcpy(t->title, title);
    t->note = malloc(strlen(note) + 1);
    strcpy(t->note, note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *t = pf->tasks, *prev = NULL;
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

void done_task(const char *pname, const char *title) {
    Project *p = find_project(pname);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *p = projects, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else projects = p->next;
            free(p->name);
            Task *t = p->tasks;
            while (t) {
                Task *tmp = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = tmp;
            }
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void report_all(void) {
    Project *p = projects;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("  %s %s %s\n", t->title, t->done ? "done" : "pending", t->note);
            t = t->next;
        }
        p = p->next;
    }
}

void free_all(void) {
    Project *p = projects;
    while (p) {
        Project *tmp = p->next;
        free(p->name);
        Task *t = p->tasks;
        while (t) {
            Task *tt = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = tt;
        }
        free(p);
        p = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[256], c[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "PROJECT") == 0) {
            if (sscanf(rest, "%255s", a) == 1) create_project(a);
        } else if (strcmp(cmd, "TASK") == 0) {
            if (sscanf(rest, "%255s %255s", a, b) == 2) {
                const char *n = rest + strlen(a) + 1 + strlen(b) + 1;
                while (*n == ' ') n++;
                if (*n) add_task(a, b, n);
                else add_task(a, b, "");
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            if (sscanf(rest, "%255s %255s %255s", a, b, c) == 3) {
                move_task(a, b, c);
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            if (sscanf(rest, "%255s %255s", a, b) == 2) done_task(a, b);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            if (sscanf(rest, "%255s", a) == 1) delete_project(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}