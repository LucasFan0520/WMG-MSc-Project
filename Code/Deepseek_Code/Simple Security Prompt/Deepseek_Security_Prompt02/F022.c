/* F022.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void project_create(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void task_add(const char *project, const char *title, const char *note) {
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

void task_move(const char *from, const char *to, const char *title) {
    Project *fp = find_project(from);
    Project *tp = find_project(to);
    if (!fp || !tp) return;
    Task *prev = NULL, *curr = fp->tasks;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else fp->tasks = curr->next;
            curr->next = tp->tasks;
            tp->tasks = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void task_done(const char *project, const char *title) {
    Project *p = find_project(project);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void project_delete(const char *name) {
    Project *prev = NULL, *curr = projects;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else projects = curr->next;
            free_project(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report_projects(void) {
    Project *p = projects;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("  %s %s %s\n", t->title, t->done ? "DONE" : "TODO", t->note);
            t = t->next;
        }
        p = p->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256], title[256], note[4096], from[256], to[256];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "PROJECT") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) project_create(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, title) != 2) continue;
            p += strlen(name) + strlen(title) + 2;
            while (*p == ' ') p++;
            task_add(name, title, p);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s %255s", from, to, title) == 3) task_move(from, to, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, title) == 2) task_done(name, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) project_delete(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }

    while (projects) {
        Project *tmp = projects;
        projects = projects->next;
        free_project(tmp);
    }
    return 0;
}