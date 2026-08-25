// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct task {
    char *title;
    char *note;
    int done;
    struct task *next;
};

struct project {
    char *name;
    struct task *tasks;
    struct project *next;
};

struct project *projects = NULL;

struct project *find_project(char *name) {
    struct project *p = projects;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

struct task *find_task_in_project(struct project *p, char *title) {
    struct task *t = p->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

struct task *find_task_anywhere(char *title) {
    struct project *p = projects;
    while (p) {
        struct task *t = find_task_in_project(p, title);
        if (t) return t;
        p = p->next;
    }
    return NULL;
}

struct project *find_project_of_task(char *title) {
    struct project *p = projects;
    while (p) {
        if (find_task_in_project(p, title)) return p;
        p = p->next;
    }
    return NULL;
}

void create_project(char *name) {
    if (find_project(name)) return;
    struct project *p = malloc(sizeof(struct project));
    p->name = malloc(strlen(name) + 1);
    strcpy(p->name, name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void add_task(char *proj, char *title, char *note) {
    struct project *p = find_project(proj);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    struct task *t = malloc(sizeof(struct task));
    t->title = malloc(strlen(title) + 1);
    strcpy(t->title, title);
    t->note = malloc(strlen(note) + 1);
    strcpy(t->note, note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(char *from, char *to, char *title) {
    struct project *pfrom = find_project(from);
    struct project *pto = find_project(to);
    if (!pfrom || !pto) return;
    struct task *t = find_task_in_project(pfrom, title);
    if (!t) return;
    if (find_task_in_project(pto, title)) return; // prevent duplicate
    // remove from from
    struct task *cur = pfrom->tasks, *prev = NULL;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else pfrom->tasks = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    // add to to
    t->next = pto->tasks;
    pto->tasks = t;
}

void done_task(char *proj, char *title) {
    struct project *p = find_project(proj);
    if (!p) return;
    struct task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void delete_project(char *name) {
    struct project *p = projects, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else projects = p->next;
            free(p->name);
            struct task *t = p->tasks;
            while (t) {
                struct task *next = t->next;
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

void report_projects() {
    struct project *p = projects;
    while (p) {
        printf("%s\n", p->name);
        struct task *t = p->tasks;
        while (t) {
            printf("%s %s %s\n", t->title, t->note, t->done ? "done" : "pending");
            t = t->next;
        }
        p = p->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[4096];
        int n = sscanf(line, "%31s %255s %255s %4095[^\n]", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "PROJECT") == 0 && n >= 2) {
            create_project(arg1);
        } else if (strcmp(cmd, "TASK") == 0 && n >= 3) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(arg2) + 1;
            add_task(arg1, arg2, rest);
        } else if (strcmp(cmd, "MOVETASK") == 0 && n >= 4) {
            move_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0 && n >= 3) {
            done_task(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0 && n >= 2) {
            delete_project(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }
    return 0;
}