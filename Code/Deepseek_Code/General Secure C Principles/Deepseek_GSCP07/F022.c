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
    Project *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

Task* find_task_in_project(Project *p, const char *title) {
    Task *t = p->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void free_tasks(Task *t) {
    while (t) {
        Task *tmp = t;
        t = t->next;
        free(tmp->title);
        free(tmp->note);
        free(tmp);
    }
}

void free_project(Project *p) {
    free(p->name);
    free_tasks(p->tasks);
    free(p);
}

void handle_project(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    if (find_project(name)) return;
    Project *new = malloc(sizeof(Project));
    new->name = strdup(name);
    new->tasks = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Project *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_task(char *cmd) {
    char *pname = strtok(NULL, " ");
    if (!pname) return;
    Project *p = find_project(pname);
    if (!p) return;
    char *title = strtok(NULL, " ");
    if (!title) return;
    char *note = strtok(NULL, "");
    if (!note) note = "";
    if (find_task_in_project(p, title)) return;
    Task *new = malloc(sizeof(Task));
    new->title = strdup(title);
    new->note = strdup(note);
    new->done = 0;
    new->next = NULL;
    if (!p->tasks) p->tasks = new;
    else {
        Task *last = p->tasks;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_movetask(char *cmd) {
    char *from = strtok(NULL, " ");
    char *to = strtok(NULL, " ");
    char *title = strtok(NULL, " ");
    if (!from || !to || !title) return;
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

void handle_donetask(char *cmd) {
    char *pname = strtok(NULL, " ");
    char *title = strtok(NULL, " ");
    if (!pname || !title) return;
    Project *p = find_project(pname);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void handle_deleteproject(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Project *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free_project(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void handle_report() {
    Project *p = head;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("%s %s %s\n", t->title, t->note, t->done ? "DONE" : "TODO");
            t = t->next;
        }
        p = p->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "PROJECT") == 0) handle_project(cmd);
        else if (strcmp(cmd, "TASK") == 0) handle_task(cmd);
        else if (strcmp(cmd, "MOVETASK") == 0) handle_movetask(cmd);
        else if (strcmp(cmd, "DONETASK") == 0) handle_donetask(cmd);
        else if (strcmp(cmd, "DELETEPROJECT") == 0) handle_deleteproject(cmd);
        else if (strcmp(cmd, "REPORT") == 0) handle_report();
    }
    while (head) {
        Project *tmp = head;
        head = head->next;
        free_project(tmp);
    }
    return 0;
}