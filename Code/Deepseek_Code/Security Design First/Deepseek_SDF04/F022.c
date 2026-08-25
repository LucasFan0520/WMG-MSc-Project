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

Project *head = NULL;

Project *find_project(const char *name) {
    Project *p = head;
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

void create_project(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) return;
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = NULL;
    if (!p->name) { free(p); return; }
    if (!head) head = p;
    else {
        Project *last = head;
        while (last->next) last = last->next;
        last->next = p;
    }
}

void add_task(const char *proj, const char *title, const char *note) {
    Project *p = find_project(proj);
    if (!p) return;
    if (find_task(p, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = NULL;
    if (!t->title || !t->note) {
        free(t->title); free(t->note); free(t);
        return;
    }
    if (!p->tasks) p->tasks = t;
    else {
        Task *last = p->tasks;
        while (last->next) last = last->next;
        last->next = t;
    }
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
            t->next = NULL;
            if (!pt->tasks) pt->tasks = t;
            else {
                Task *last = pt->tasks;
                while (last->next) last = last->next;
                last->next = t;
            }
            return;
        }
        prev = t;
        t = t->next;
    }
}

void done_task(const char *proj, const char *title) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *t = find_task(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name);
            Task *t = p->tasks;
            while (t) {
                Task *next = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = next;
            }
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void report_all(void) {
    Project *p = head;
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

void free_all(void) {
    Project *p = head;
    while (p) {
        Project *next = p->next;
        free(p->name);
        Task *t = p->tasks;
        while (t) {
            Task *tnext = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = tnext;
        }
        free(p);
        p = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], p1[256], p2[256], title[256], note[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            if (sscanf(line, "%*s %255s", p1) == 1)
                create_project(p1);
        } else if (strcmp(cmd, "TASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", p1, title, note) == 3)
                add_task(p1, title, note);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", p1, p2, title) == 3)
                move_task(p1, p2, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s", p1, title) == 2)
                done_task(p1, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            if (sscanf(line, "%*s %255s", p1) == 1)
                delete_project(p1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}