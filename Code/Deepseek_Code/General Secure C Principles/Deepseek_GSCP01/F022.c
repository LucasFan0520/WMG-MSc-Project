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

Task *find_task_in_project(Project *p, const char *title) {
    Task *t = p->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_project(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void add_task(const char *project, const char *title, const char *note) {
    Project *p = find_project(project);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *fp = find_project(from);
    Project *tp = find_project(to);
    if (!fp || !tp) return;
    Task *t = fp->tasks, *prev = NULL;
    while (t && strcmp(t->title, title) != 0) {
        prev = t;
        t = t->next;
    }
    if (!t) return;
    if (prev) prev->next = t->next;
    else fp->tasks = t->next;
    t->next = tp->tasks;
    tp->tasks = t;
}

void done_task(const char *project, const char *title) {
    Project *p = find_project(project);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (!t) return;
    t->done = 1;
}

void delete_project(const char *name) {
    Project *p = projects, *prev = NULL;
    while (p && strcmp(p->name, name) != 0) {
        prev = p;
        p = p->next;
    }
    if (!p) return;
    if (prev) prev->next = p->next;
    else projects = p->next;
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
}

void report_all(void) {
    Project *p = projects;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("%s ", t->title);
            if (t->done) printf("done ");
            for (char *c = t->note; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
            t = t->next;
        }
        p = p->next;
    }
}

void free_all(void) {
    Project *p = projects;
    while (p) {
        Project *nextp = p->next;
        free(p->name);
        Task *t = p->tasks;
        while (t) {
            Task *nextt = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = nextt;
        }
        free(p);
        p = nextp;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            add_project(p);
        } else if (strcmp(cmd, "TASK") == 0) {
            char project[256], title[256];
            char *p = line + 5;
            if (sscanf(p, "%255s %255s", project, title) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_task(project, title, p);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256], to[256], title[256];
            if (sscanf(line + 9, "%255s %255s %255s", from, to, title) == 3)
                move_task(from, to, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char project[256], title[256];
            if (sscanf(line + 9, "%255s %255s", project, title) == 2)
                done_task(project, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *p = line + 14;
            while (*p == ' ') p++;
            delete_project(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    free(line);
    return 0;
}