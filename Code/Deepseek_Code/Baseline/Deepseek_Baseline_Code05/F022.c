/* F022.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct task {
    char *title;
    char *note;
    struct task *next;
} Task;

typedef struct project {
    char *name;
    Task *tasks;
    struct project *next;
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
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void add_task(const char *pname, const char *title, const char *note) {
    Project *p = find_project(pname);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *fp = find_project(from);
    Project *tp = find_project(to);
    if (!fp || !tp) return;
    Task *t = fp->tasks, *prev = NULL;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else fp->tasks = t->next;
            t->next = tp->tasks;
            tp->tasks = t;
            return;
        }
        prev = t;
        t = t->next;
    }
}

void done_task(const char *pname, const char *title) {
    Project *p = find_project(pname);
    if (!p) return;
    Task *t = p->tasks, *prev = NULL;
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
    Project *p = projects, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else projects = p->next;
            free(p->name);
            Task *t = p->tasks;
            while (t) { Task *tmp = t; t = t->next; free(tmp->title); free(tmp->note); free(tmp); }
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void report() {
    Project *p = projects;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("%s ", t->title);
            for (char *c = t->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
            t = t->next;
        }
        p = p->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], name2[256], title[256], note[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report(); continue; }
        if (strcmp(cmd, "PROJECT") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) create_project(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, title) < 2) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1 + strlen(title) + 1;
            if (*rest == ' ') rest++;
            add_task(name, title, rest);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", name, name2, title) == 3)
                move_task(name, name2, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, title) == 2)
                done_task(name, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_project(name);
        }
    }
    return 0;
}