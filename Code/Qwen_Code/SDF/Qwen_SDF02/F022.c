// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

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

Project* find_project(const char *name) {
    for (Project *p = projects; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

Task* find_task(Project *p, const char *title) {
    for (Task *t = p->tasks; t; t = t->next)
        if (strcmp(t->title, title) == 0) return t;
    return NULL;
}

Task* remove_task(Project *p, const char *title) {
    Task *prev = NULL;
    for (Task *t = p->tasks; t; prev = t, t = t->next) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else p->tasks = t->next;
            t->next = NULL;
            return t;
        }
    }
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "PROJECT") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            if (find_project(name)) continue;
            Project *p = malloc(sizeof(Project));
            if (!p) continue;
            p->name = strdup(name);
            if (!p->name) { free(p); continue; }
            p->tasks = NULL;
            p->next = projects;
            projects = p;
        } else if (strcmp(cmd, "TASK") == 0) {
            char proj[256], title[256];
            int n = 0;
            if (sscanf(rest, "%255s %255s%n", proj, title, &n) != 2) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            Project *p = find_project(proj);
            if (!p || find_task(p, title)) continue;
            Task *t = malloc(sizeof(Task));
            if (!t) continue;
            t->title = strdup(title);
            t->note = strdup(note);
            if (!t->title || !t->note) { free(t->title); free(t->note); free(t); continue; }
            t->done = 0;
            t->next = p->tasks;
            p->tasks = t;
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256], to[256], title[256];
            if (sscanf(rest, "%255s %255s %255s", from, to, title) != 3) continue;
            Project *p1 = find_project(from);
            Project *p2 = find_project(to);
            if (!p1 || !p2) continue;
            Task *t = remove_task(p1, title);
            if (t) {
                t->next = p2->tasks;
                p2->tasks = t;
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char proj[256], title[256];
            if (sscanf(rest, "%255s %255s", proj, title) != 2) continue;
            Project *p = find_project(proj);
            if (p) {
                Task *t = find_task(p, title);
                if (t) t->done = 1;
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Project *prev = NULL;
            for (Project *p = projects; p; prev = p, p = p->next) {
                if (strcmp(p->name, name) == 0) {
                    if (prev) prev->next = p->next;
                    else projects = p->next;
                    while (p->tasks) {
                        Task *t = p->tasks;
                        p->tasks = t->next;
                        free(t->title);
                        free(t->note);
                        free(t);
                    }
                    free(p->name);
                    free(p);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Project *p = projects; p; p = p->next) {
                printf("%s\n", p->name);
                for (Task *t = p->tasks; t; t = t->next) {
                    printf("  %s %s ", t->title, t->done ? "DONE" : "TODO");
                    print_text(t->note);
                }
            }
        }
    }
    while (projects) {
        Project *p = projects;
        projects = projects->next;
        while (p->tasks) {
            Task *t = p->tasks;
            p->tasks = t->next;
            free(t->title);
            free(t->note);
            free(t);
        }
        free(p->name);
        free(p);
    }
    return 0;
}