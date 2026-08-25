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

Project *find_proj(const char *name) {
    for (Project *p = head; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

void add_proj(const char *name) {
    if (find_proj(name)) return;
    Project *p = malloc(sizeof(Project));
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = head;
    head = p;
}

void add_task(const char *pname, const char *title, const char *note) {
    Project *p = find_proj(pname);
    if (!p) return;
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *pf = find_proj(from);
    Project *pt = find_proj(to);
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
        prev = t; t = t->next;
    }
}

void done_task(const char *pname, const char *title) {
    Project *p = find_proj(pname);
    if (!p) return;
    for (Task *t = p->tasks; t; t = t->next) {
        if (strcmp(t->title, title) == 0) {
            t->done = 1;
            return;
        }
    }
}

void delete_proj(const char *name) {
    Project *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            Task *t = p->tasks;
            while (t) {
                Task *nt = t->next;
                free(t->title); free(t->note); free(t);
                t = nt;
            }
            free(p->name); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void report() {
    for (Project *p = head; p; p = p->next) {
        printf("%s\n", p->name);
        for (Task *t = p->tasks; t; t = t->next) {
            printf("  %s %s %s\n", t->title, t->done ? "DONE" : "TODO", t->note);
        }
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[1000], c[8000];
        c[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "PROJECT") == 0) {
            sscanf(line, "%*s %s", a);
            add_proj(a);
        } else if (strcmp(cmd, "TASK") == 0) {
            sscanf(line, "%*s %s %s %[^\n]", a, b, c);
            add_task(a, b, c);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            sscanf(line, "%*s %s %s %s", a, b, c);
            move_task(a, b, c);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            done_task(a, b);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            sscanf(line, "%*s %s", a);
            delete_proj(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    while (head) {
        Project *n = head->next;
        Task *t = head->tasks;
        while (t) {
            Task *nt = t->next;
            free(t->title); free(t->note); free(t);
            t = nt;
        }
        free(head->name); free(head);
        head = n;
    }
    return 0;
}