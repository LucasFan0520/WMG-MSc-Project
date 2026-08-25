// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_TITLE 64
#define MAX_NOTE 4096

typedef struct Task {
    char title[MAX_TITLE];
    char note[MAX_NOTE];
    int done;
    struct Task *next;
} Task;

typedef struct Project {
    char name[MAX_NAME];
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

void free_tasks(Task *t) {
    while (t) {
        Task *tmp = t;
        t = t->next;
        free(tmp);
    }
}

void free_all() {
    Project *p = head;
    while (p) {
        Project *tmp = p;
        free_tasks(p->tasks);
        p = p->next;
        free(tmp);
    }
    head = NULL;
}

void handle_project(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "PROJECT %63s", name) != 1) return;
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) return;
    strcpy(p->name, name);
    p->tasks = NULL;
    p->next = head;
    head = p;
}

void handle_task(char *line) {
    char pname[MAX_NAME], title[MAX_TITLE], note[MAX_NOTE];
    int n = sscanf(line, "TASK %63s %63s %4095[^\n]", pname, title, note);
    if (n < 3) return;
    Project *p = find_project(pname);
    if (!p) return;
    if (find_task(p, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    strcpy(t->title, title);
    strcpy(t->note, note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void handle_movetask(char *line) {
    char from[MAX_NAME], to[MAX_NAME], title[MAX_TITLE];
    if (sscanf(line, "MOVETASK %63s %63s %63s", from, to, title) != 3) return;
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *t = find_task(pf, title);
    if (!t) return;
    // remove from pf
    Task *cur = pf->tasks, *prev = NULL;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else pf->tasks = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    // add to pt
    t->next = pt->tasks;
    pt->tasks = t;
}

void handle_donetask(char *line) {
    char pname[MAX_NAME], title[MAX_TITLE];
    if (sscanf(line, "DONETASK %63s %63s", pname, title) != 2) return;
    Project *p = find_project(pname);
    if (!p) return;
    Task *t = find_task(p, title);
    if (!t) return;
    t->done = 1;
}

void handle_deleteproject(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DELETEPROJECT %63s", name) != 1) return;
    Project *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free_tasks(p->tasks);
            free(p);
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
            printf("  %s %s", t->title, t->note);
            if (t->done) printf(" DONE");
            putchar('\n');
            t = t->next;
        }
        p = p->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "PROJECT ", 8) == 0) handle_project(line);
        else if (strncmp(line, "TASK ", 5) == 0) handle_task(line);
        else if (strncmp(line, "MOVETASK ", 9) == 0) handle_movetask(line);
        else if (strncmp(line, "DONETASK ", 9) == 0) handle_donetask(line);
        else if (strncmp(line, "DELETEPROJECT ", 14) == 0) handle_deleteproject(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}