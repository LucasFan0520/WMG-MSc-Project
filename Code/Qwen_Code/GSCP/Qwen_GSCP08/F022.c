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

Project *find_project(char *name) {
    for (Project *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) return p;
    }
    return NULL;
}

Task *find_task(Project *p, char *title) {
    for (Task *t = p->tasks; t; t = t->next) {
        if (strcmp(t->title, title) == 0) return t;
    }
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

void add_task(char *pname, char *title, char *note) {
    Project *p = find_project(pname);
    if (!p) return;
    if (find_task(p, title)) return;
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(char *from, char *to, char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *prev = NULL;
    Task *curr = pf->tasks;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else pf->tasks = curr->next;
            curr->next = pt->tasks;
            pt->tasks = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void done_task(char *pname, char *title) {
    Project *p = find_project(pname);
    if (!p) return;
    Task *t = find_task(p, title);
    if (t) t->done = 1;
}

void delete_project(char *name) {
    Project *prev = NULL;
    Project *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            Task *t = curr->tasks;
            while (t) {
                Task *next = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report_projects() {
    for (Project *p = head; p; p = p->next) {
        printf("%s\n", p->name);
        for (Task *t = p->tasks; t; t = t->next) {
            printf("  %s %s %s\n", t->title, t->done ? "DONE" : "TODO", t->note);
        }
    }
}

int main() {
    char cmd[20], a[100], b[100], c[100], note[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "PROJECT") == 0) {
            scanf("%99s", a);
            create_project(a);
        } else if (strcmp(cmd, "TASK") == 0) {
            scanf("%99s%99s", a, b);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            add_task(a, b, note);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            scanf("%99s%99s%99s", a, b, c);
            move_task(a, b, c);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            scanf("%99s%99s", a, b);
            done_task(a, b);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            scanf("%99s", a);
            delete_project(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }
    return 0;
}