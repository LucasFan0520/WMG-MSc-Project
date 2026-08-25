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

Project *projects = NULL;

Project *find_project(const char *name) {
    Project *p = projects;
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

void remove_task_from_list(Project *p, Task *t) {
    Task *cur = p->tasks, *prev = NULL;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else p->tasks = cur->next;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void project_create(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) return;
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void task_add(const char *pname, const char *title, const char *note) {
    Project *p = find_project(pname);
    if (!p) return;
    if (find_task(p, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void task_move(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *t = find_task(pf, title);
    if (!t) return;
    remove_task_from_list(pf, t);
    t->next = pt->tasks;
    pt->tasks = t;
}

void task_done(const char *pname, const char *title) {
    Project *p = find_project(pname);
    if (!p) return;
    Task *t = find_task(p, title);
    if (t) t->done = 1;
}

void project_delete(const char *name) {
    Project *p = projects, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else projects = p->next;
            free(p->name);
            while (p->tasks) {
                Task *tmp = p->tasks;
                p->tasks = p->tasks->next;
                free(tmp->title);
                free(tmp->note);
                free(tmp);
            }
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void report_all(void) {
    Project *p = projects;
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%15s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "PROJECT") == 0 && n >= 2) {
            project_create(arg1);
        } else if (strcmp(cmd, "TASK") == 0 && n >= 3) {
            char *note = line + strlen(cmd) + strlen(arg1) + strlen(arg2) + 3;
            while (*note == ' ') note++;
            task_add(arg1, arg2, note);
        } else if (strcmp(cmd, "MOVETASK") == 0 && n >= 4) {
            task_move(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0 && n >= 3) {
            task_done(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0 && n >= 2) {
            project_delete(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    while (projects) {
        Project *tmp = projects;
        projects = projects->next;
        free(tmp->name);
        while (tmp->tasks) {
            Task *t = tmp->tasks;
            tmp->tasks = tmp->tasks->next;
            free(t->title);
            free(t->note);
            free(t);
        }
        free(tmp);
    }
    return 0;
}