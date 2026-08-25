// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

typedef struct Project {
    char *name;
    Task *tasks;
    struct Project *next;
} Project;

Project *head = NULL;
Project *tail = NULL;

Project *find_project(const char *name) {
    Project *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void create_project(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    p->name = malloc(strlen(name)+1);
    strcpy(p->name, name);
    p->tasks = NULL;
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void add_task(const char *proj, const char *title, const char *note) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *t = malloc(sizeof(Task));
    t->title = malloc(strlen(title)+1);
    strcpy(t->title, title);
    t->note = malloc(strlen(note)+1);
    strcpy(t->note, note);
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *prev = NULL;
    Task *t = pf->tasks;
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

void done_task(const char *proj, const char *title) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *prev = NULL;
    Task *t = p->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else p->tasks = t->next;
            free(t->title);
            free(t->note);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void delete_project(const char *name) {
    Project *prev = NULL;
    Project *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (p == tail) tail = prev;
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
            printf("%s %s\n", t->title, t->note);
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = strtok(rest, " ");
            if (name) create_project(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            char *proj = strtok(rest, " ");
            if (!proj) continue;
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_task(proj, title, note);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *from = strtok(rest, " ");
            char *to = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (from && to && title) move_task(from, to, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *proj = strtok(rest, " ");
            if (!proj) continue;
            char *title = strtok(NULL, " ");
            if (title) done_task(proj, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_project(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}