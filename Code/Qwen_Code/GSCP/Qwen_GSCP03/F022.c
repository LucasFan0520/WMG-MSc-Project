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

void free_tasks(Task *t) {
    while (t) { Task *tmp = t; t = t->next; free(tmp->title); free(tmp->note); free(tmp); }
}

void free_list() {
    while (head) {
        Project *p = head; head = head->next;
        free(p->name); free_tasks(p->tasks); free(p);
    }
}

Project *find_project(const char *name) {
    for (Project *p = head; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

Task *find_task(Project *p, const char *title) {
    for (Task *t = p->tasks; t; t = t->next)
        if (strcmp(t->title, title) == 0) return t;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], p1[256], p2[256], title[256], note[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "PROJECT") == 0) {
            if (sscanf(line, "%*s %255s", p1) != 1) continue;
            if (find_project(p1)) continue;
            Project *p = malloc(sizeof(Project));
            p->name = strdup(p1); p->tasks = NULL; p->next = head; head = p;
        } else if (strcmp(cmd, "TASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", p1, title, note) < 2) continue;
            Project *p = find_project(p1);
            if (p && !find_task(p, title)) {
                Task *t = malloc(sizeof(Task));
                t->title = strdup(title); t->note = strdup(note); t->done = 0;
                t->next = p->tasks; p->tasks = t;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", p1, p2, title) != 3) continue;
            Project *src = find_project(p1), *dst = find_project(p2);
            if (src && dst) {
                Task *prev = NULL, *curr = src->tasks;
                while (curr) {
                    if (strcmp(curr->title, title) == 0) {
                        if (prev) prev->next = curr->next; else src->tasks = curr->next;
                        curr->next = dst->tasks; dst->tasks = curr;
                        break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            if (sscanf(line, "%*s %255s %255s", p1, title) != 2) continue;
            Project *p = find_project(p1);
            if (p) {
                Task *t = find_task(p, title);
                if (t) t->done = 1;
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            if (sscanf(line, "%*s %255s", p1) != 1) continue;
            Project *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, p1) == 0) {
                    Project *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free_tasks(tmp->tasks); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Project *p = head; p; p = p->next) {
                printf("%s:\n", p->name);
                for (Task *t = p->tasks; t; t = t->next)
                    printf("  %s %s %s\n", t->title, t->done ? "DONE" : "TODO", t->note);
            }
        }
    }
    free_list();
    return 0;
}