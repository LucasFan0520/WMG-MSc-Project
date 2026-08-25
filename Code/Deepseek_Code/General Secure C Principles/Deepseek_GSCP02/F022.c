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
    Task *task_head;
    struct Project *next;
} Project;

Project *p_head = NULL;

Project *find_project(const char *name) {
    Project *p = p_head;
    while (p) {
        if (strcmp(p->name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

Task *find_task_in_project(Project *p, const char *title) {
    Task *t = p->task_head;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void create_project(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) return;
    p->name = strdup(name);
    p->task_head = NULL;
    p->next = p_head;
    p_head = p;
}

void add_task(const char *pname, const char *title, const char *note) {
    Project *p = find_project(pname);
    if (!p) return;
    if (find_task_in_project(p, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->done = 0;
    t->next = p->task_head;
    p->task_head = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *pf = find_project(from);
    Project *pt = find_project(to);
    if (!pf || !pt) return;
    if (find_task_in_project(pt, title)) return;
    Task *prev = NULL;
    Task *t = pf->task_head;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else pf->task_head = t->next;
            t->next = pt->task_head;
            pt->task_head = t;
            return;
        }
        prev = t;
        t = t->next;
    }
}

void done_task(const char *pname, const char *title) {
    Project *p = find_project(pname);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *prev = NULL;
    Project *p = p_head;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else p_head = p->next;
            free(p->name);
            Task *t = p->task_head;
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
    Project *p = p_head;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->task_head;
        while (t) {
            printf("%s %s ", t->title, t->done ? "done" : "pending");
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
    while (p_head) {
        Project *next = p_head->next;
        free(p_head->name);
        Task *t = p_head->task_head;
        while (t) {
            Task *nextt = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = nextt;
        }
        free(p_head);
        p_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "PROJECT") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                create_project(name);
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char pname[256], title[256], note[1024];
            if (sscanf(line, "%*s %255s %255s %1023[^\n]", pname, title, note) == 3) {
                add_task(pname, title, note);
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char from[256], to[256], title[256];
            if (sscanf(line, "%*s %255s %255s %255s", from, to, title) == 3) {
                move_task(from, to, title);
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char pname[256], title[256];
            if (sscanf(line, "%*s %255s %255s", pname, title) == 2) {
                done_task(pname, title);
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_project(name);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }

    free(line);
    free_all();
    return 0;
}