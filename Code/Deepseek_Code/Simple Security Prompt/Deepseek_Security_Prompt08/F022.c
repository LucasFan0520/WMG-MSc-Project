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

Project *head = NULL, *tail = NULL;

Project *find_project(const char *name) {
    Project *p = head;
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
    p->name = malloc(strlen(name) + 1);
    strcpy(p->name, name);
    p->tasks = NULL;
    p->next = NULL;
    if (tail) {
        tail->next = p;
        tail = p;
    } else {
        head = tail = p;
    }
}

void add_task(const char *proj, const char *title, const char *note) {
    Project *p = find_project(proj);
    if (!p) return;
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

void done_task(const char *proj, const char *title) {
    Project *p = find_project(proj);
    if (!p) return;
    Task *t = find_task_in_project(p, title);
    if (t) t->done = 1;
}

void delete_project(const char *name) {
    Project *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
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

void report_projects(void) {
    Project *p = head;
    while (p) {
        printf("%s\n", p->name);
        Task *t = p->tasks;
        while (t) {
            printf("%s %s %s\n", t->title, t->note, t->done ? "DONE" : "TODO");
            t = t->next;
        }
        p = p->next;
    }
}

void free_all(void) {
    Project *p = head;
    while (p) {
        Project *next = p->next;
        Task *t = p->tasks;
        while (t) {
            Task *nextt = t->next;
            free(t->title);
            free(t->note);
            free(t);
            t = nextt;
        }
        free(p->name);
        free(p);
        p = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) create_project(name);
        } else if (strcmp(cmd, "TASK") == 0) {
            char *proj = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (proj && title && note) add_task(proj, title, note);
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *from = strtok(NULL, " ");
            char *to = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (from && to && title) move_task(from, to, title);
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *proj = strtok(NULL, " ");
            char *title = strtok(NULL, " ");
            if (proj && title) done_task(proj, title);
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_project(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_projects();
        }
    }
    free(line);
    free_all();
    return 0;
}