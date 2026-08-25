// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

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

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
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

void free_project(Project *p) {
    Task *t = p->tasks;
    while (t) {
        Task *next = t->next;
        free(t->title);
        free(t->note);
        free(t);
        t = next;
    }
    free(p->name);
    free(p);
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = get_token(&ptr);
            if (name && !find_project(name)) {
                Project *p = malloc(sizeof(Project));
                if (p) {
                    p->name = strdup(name);
                    p->tasks = NULL;
                    p->next = head;
                    head = p;
                }
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *pname = get_token(&ptr);
            char *title = get_token(&ptr);
            if (pname && title) {
                Project *p = find_project(pname);
                if (p && !find_task(p, title)) {
                    Task *t = malloc(sizeof(Task));
                    if (t) {
                        t->title = strdup(title);
                        t->note = strdup(ptr);
                        t->done = 0;
                        t->next = p->tasks;
                        p->tasks = t;
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *from = get_token(&ptr);
            char *to = get_token(&ptr);
            char *title = get_token(&ptr);
            if (from && to && title) {
                Project *pf = find_project(from);
                Project *pt = find_project(to);
                if (pf && pt) {
                    Task *prev = NULL, *curr = pf->tasks;
                    while (curr) {
                        if (strcmp(curr->title, title) == 0) {
                            if (prev) prev->next = curr->next;
                            else pf->tasks = curr->next;
                            curr->next = pt->tasks;
                            pt->tasks = curr;
                            break;
                        }
                        prev = curr;
                        curr = curr->next;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *pname = get_token(&ptr);
            char *title = get_token(&ptr);
            if (pname && title) {
                Project *p = find_project(pname);
                if (p) {
                    Task *t = find_task(p, title);
                    if (t) t->done = 1;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Project *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free_project(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Project *p = head; p; p = p->next) {
                printf("%s\n", p->name);
                for (Task *t = p->tasks; t; t = t->next) {
                    printf("  %s %s %s\n", t->title, t->done ? "done" : "pending", t->note);
                }
            }
        }
    }
    while (head) {
        Project *next = head->next;
        free_project(head);
        head = next;
    }
    return 0;
}