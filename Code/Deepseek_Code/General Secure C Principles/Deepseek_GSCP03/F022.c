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

void project_create(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) return;
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = head;
    head = p;
}

void task_add(const char *project, const char *title, const char *note) {
    Project *p = find_project(project);
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
    Project *fp = find_project(from);
    Project *tp = find_project(to);
    if (!fp || !tp) return;
    Task *t = find_task(fp, title);
    if (!t) return;
    if (find_task(tp, title)) return;
    // remove from fp
    Task *prev = NULL, *curr = fp->tasks;
    while (curr) {
        if (curr == t) {
            if (prev) prev->next = curr->next;
            else fp->tasks = curr->next;
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    // add to tp
    t->next = tp->tasks;
    tp->tasks = t;
}

void task_done(const char *project, const char *title) {
    Project *p = find_project(project);
    if (!p) return;
    Task *t = find_task(p, title);
    if (t) t->done = 1;
}

void project_delete(const char *name) {
    Project *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            Task *t = curr->tasks;
            while (t) {
                Task *tmp = t;
                t = t->next;
                free(tmp->title);
                free(tmp->note);
                free(tmp);
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void project_report(void) {
    Project *p = head;
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

void free_all(void) {
    Project *p = head;
    while (p) {
        Project *tmp = p;
        p = p->next;
        free(tmp->name);
        Task *t = tmp->tasks;
        while (t) {
            Task *tmp_t = t;
            t = t->next;
            free(tmp_t->title);
            free(tmp_t->note);
            free(tmp_t);
        }
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            project_create(p);
        } else if (strcmp(cmd, "TASK") == 0) {
            char proj[256], title[256];
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", proj, title) == 2) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    task_add(proj, title, note);
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256], to[256], title[256];
            if (sscanf(line + 9, "%255s %255s %255s", from, to, title) == 3) {
                task_move(from, to, title);
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char proj[256], title[256];
            if (sscanf(line + 9, "%255s %255s", proj, title) == 2) {
                task_done(proj, title);
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *p = line + 14;
            while (*p == ' ') p++;
            project_delete(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            project_report();
        }
    }
    free_all();
    return 0;
}