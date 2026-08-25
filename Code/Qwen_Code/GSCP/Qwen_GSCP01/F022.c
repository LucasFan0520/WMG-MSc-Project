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

Project *projects = NULL;

Project *find_project(const char *name) {
    Project *curr = projects;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

Task *find_task(Project *p, const char *title) {
    Task *curr = p->tasks;
    while (curr) {
        if (strcmp(curr->title, title) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void create_project(const char *name) {
    if (find_project(name)) return;
    Project *p = malloc(sizeof(Project));
    if (!p) exit(EXIT_FAILURE);
    p->name = strdup(name);
    p->tasks = NULL;
    p->next = projects;
    projects = p;
}

void add_task(const char *pname, const char *title, const char *note) {
    Project *p = find_project(pname);
    if (!p) return;
    if (find_task(p, title)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) exit(EXIT_FAILURE);
    t->title = strdup(title);
    t->note = strdup(note);
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
    Task *t = find_task(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *curr = projects;
    Project *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else projects = curr->next;
            Task *t = curr->tasks;
            while (t) {
                Task *next_t = t->next;
                free(t->title);
                free(t->note);
                free(t);
                t = next_t;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report_projects() {
    Project *p = projects;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("%s %s %s\n", t->title, t->note, t->done ? "DONE" : "");
            t = t->next;
        }
        p = p->next;
    }
}

void free_all() {
    Project *curr = projects;
    while (curr) {
        Project *next_p = curr->next;
        Task *t = curr->tasks;
        while (t) {
            Task *next_t = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = next_t;
        }
        free(curr->name);
        free(curr);
        curr = next_p;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[256] = {0};
        char arg4[3200] = {0};
        
        int matched = sscanf(line, "%31s %255s %255s %255s %[^\n]", cmd, arg1, arg2, arg3, arg4);
        
        if (strcmp(cmd, "PROJECT") == 0) {
            create_project(arg1);
        } else if (strcmp(cmd, "TASK") == 0 && matched >= 4) {
            add_task(arg1, arg2, arg4);
        } else if (strcmp(cmd, "MOVETASK") == 0 && matched >= 3) {
            move_task(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DONETASK") == 0 && matched >= 2) {
            done_task(arg1, arg2);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            delete_project(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }
    free_all();
    return 0;
}