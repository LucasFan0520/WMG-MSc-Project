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

Project* find_project(const char *name) {
    Project *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

Task* find_task(Project *p, const char *title) {
    Task *cur = p->tasks;
    while (cur) {
        if (strcmp(cur->title, title) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_project(const char *name) {
    if (find_project(name)) return;
    Project *new = malloc(sizeof(Project));
    new->name = strdup(name);
    new->tasks = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Project *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void add_task(const char *proj, const char *title, const char *note) {
    Project *p = find_project(proj);
    if (!p) return;
    if (find_task(p, title)) return;
    Task *new = malloc(sizeof(Task));
    new->title = strdup(title);
    new->note = strdup(note);
    new->done = 0;
    new->next = NULL;
    if (!p->tasks) p->tasks = new;
    else {
        Task *cur = p->tasks;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void move_task(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *prev = NULL, *cur = pf->tasks;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else pf->tasks = cur->next;
            cur->next = NULL;
            if (!pt->tasks) pt->tasks = cur;
            else {
                Task *t = pt->tasks;
                while (t->next) t = t->next;
                t->next = cur;
            }
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void done_task(const char *proj, const char *title) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *t = find_task(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
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

void report() {
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

void free_all() {
    Project *cur = head;
    while (cur) {
        Project *tmp = cur;
        cur = cur->next;
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) add_project(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            char *proj = strtok(NULL, " ");
            if (!proj) continue;
            char *title = strtok(NULL, " ");
            if (!title) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_task(proj, title, note);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *from = strtok(NULL, " ");
            if (!from) continue;
            char *to = strtok(NULL, " ");
            if (!to) continue;
            char *title = strtok(NULL, " ");
            if (title) move_task(from, to, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *proj = strtok(NULL, " ");
            if (!proj) continue;
            char *title = strtok(NULL, " ");
            if (title) done_task(proj, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_project(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}