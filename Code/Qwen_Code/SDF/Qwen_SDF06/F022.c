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
    for (Project *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) return p;
    }
    return NULL;
}

void add_project(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) return;
    p->name = strdup(name);
    if (!p->name) { free(p); return; }
    p->tasks = NULL;
    p->next = head;
    head = p;
}

void add_task(const char *pname, const char *title, const char *note) {
    Project *p = find_project(pname);
    if (!p) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    if (!t->title || !t->note) { free(t->title); free(t->note); free(t); return; }
    t->done = 0;
    t->next = p->tasks;
    p->tasks = t;
}

void move_task(const char *from, const char *to, const char *title) {
    Project *p_from = find_project(from);
    Project *p_to = find_project(to);
    if (!p_from || !p_to) return;
    Task *curr = p_from->tasks;
    Task *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else p_from->tasks = curr->next;
            curr->next = p_to->tasks;
            p_to->tasks = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void done_task(const char *pname, const char *title) {
    Project *p = find_project(pname);
    if (!p) return;
    for (Task *t = p->tasks; t; t = t->next) {
        if (strcmp(t->title, title) == 0) {
            t->done = 1;
            return;
        }
    }
}

void delete_project(const char *name) {
    Project *curr = head;
    Project *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            Task *t = curr->tasks;
            while (t) {
                Task *next = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = next;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report() {
    for (Project *p = head; p; p = p->next) {
        printf("%s\n", p->name);
        for (Task *t = p->tasks; t; t = t->next) {
            printf("%s %s %s\n", t->title, t->done ? "done" : "pending", t->note);
        }
    }
}

void free_all() {
    Project *p = head;
    while (p) {
        Project *next_p = p->next;
        Task *t = p->tasks;
        while (t) {
            Task *next_t = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = next_t;
        }
        free(p->name);
        free(p);
        p = next_p;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[1024] = {0};
        char arg3[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %1023s %4095[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        
        if (strcmp(cmd, "PROJECT") == 0 && n >= 2) {
            add_project(arg1);
        } else if (strcmp(cmd, "TASK") == 0 && n >= 4) {
            add_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "MOVETASK") == 0 && n >= 4) {
            move_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0 && n >= 3) {
            done_task(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0 && n >= 2) {
            delete_project(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}