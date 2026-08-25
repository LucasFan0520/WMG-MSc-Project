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

Project *head = NULL;

Project *find_project(char *name) {
    for (Project *p = head; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

Task *find_task_in_project(Project *p, char *title) {
    for (Task *t = p->tasks; t; t = t->next)
        if (strcmp(t->title, title) == 0) return t;
    return NULL;
}

void create_project(char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = head;
    head = p;
}

void add_task(char *proj, char *title, char *note) {
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

void move_task(char *fromProj, char *toProj, char *title) {
    Project *pf = find_project(fromProj);
    Project *pt = find_project(toProj);
    if (!pf || !pt) return;
    Task *prev = NULL;
    Task *t = pf->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) break;
        prev = t;
        t = t->next;
    }
    if (!t) return;
    if (prev) prev->next = t->next;
    else pf->tasks = t->next;
    t->next = pt->tasks;
    pt->tasks = t;
}

void done_task(char *proj, char *title) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void delete_project(char *name) {
    Project *prev = NULL;
    for (Project *p = head; p; p = p->next) {
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
    }
}

void report_all(void) {
    for (Project *p = head; p; p = p->next) {
        printf("%s\n", p->name);
        for (Task *t = p->tasks; t; t = t->next)
            printf("  %s %s %s\n", t->title, t->note, t->done ? "done" : "");
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[1024];
        int n = sscanf(line, "%15s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "PROJECT") == 0 && n == 2) {
            create_project(arg1);
        } else if (strcmp(cmd, "TASK") == 0 && n == 4) {
            add_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "MOVETASK") == 0 && n == 4) {
            move_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0 && n == 3) {
            done_task(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0 && n == 2) {
            delete_project(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}