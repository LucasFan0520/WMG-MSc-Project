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

Project *find_project(const char *name) {
    Project *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void create_project(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    p->name = malloc(strlen(name) + 1);
    strcpy(p->name, name);
    p->tasks = NULL;
    p->next = head;
    head = p;
}

Task *find_task_in_project(Project *p, const char *title) {
    Task *t = p->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_task(const char *proj, const char *title, const char *note) {
    Project *p = find_project(proj);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    Task *t = malloc(sizeof(Task));
    t->title = malloc(strlen(title) + 1);
    strcpy(t->title, title);
    t->note = malloc(strlen(note) + 1);
    strcpy(t->note, note);
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    Task *prev = NULL;
    Task *t = pf->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else pf->tasks = t->next;
            t->next = pt->tasks;
            pt->tasks = t;
            return;
        }
        prev = t;
        t = t->next;
    }
}

void done_task(const char *proj, const char *title) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *prev = NULL;
    Project *p = head;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
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
            return;
        }
        prev = p;
        p = p->next;
    }
}

void report_all(void) {
    Project *p = head;
    while (p) {
        printf("%s:\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("  %s [%s] ", t->title, t->done ? "done" : "todo");
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
    Project *p = head;
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            create_project(p);
        } else if (strcmp(cmd, "TASK") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_task(proj, title, p);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *from = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *to = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            move_task(from, to, p);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *proj = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            done_task(proj, p);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *p = line + 14;
            while (*p == ' ') p++;
            delete_project(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}