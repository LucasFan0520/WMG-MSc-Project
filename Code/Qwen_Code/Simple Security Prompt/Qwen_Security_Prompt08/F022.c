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

typedef struct Proj {
    char *name;
    Task *tasks;
    struct Proj *next;
} Proj;

Proj *head = NULL;

Proj* find_proj(char *name) {
    for (Proj *p = head; p; p = p->next) {
        if (!strcmp(p->name, name)) return p;
    }
    return NULL;
}

int main() {
    char cmd[20];
    char name[100];
    char title[100];
    char note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "PROJECT")) {
            scanf("%s", name);
            Proj *p = malloc(sizeof(Proj));
            p->name = strdup(name);
            p->tasks = NULL;
            p->next = head;
            head = p;
        } else if (!strcmp(cmd, "TASK")) {
            scanf("%s %s", name, title);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            Proj *p = find_proj(name);
            if (p) {
                Task *t = malloc(sizeof(Task));
                t->title = strdup(title);
                t->note = strdup(note);
                t->done = 0;
                t->next = p->tasks;
                p->tasks = t;
            }
        } else if (!strcmp(cmd, "MOVETASK")) {
            char from[100], to[100];
            scanf("%s %s %s", from, to, title);
            Proj *pf = find_proj(from);
            Proj *pt = find_proj(to);
            if (pf && pt) {
                Task *p = pf->tasks;
                Task *prev = NULL;
                while (p) {
                    if (!strcmp(p->title, title)) {
                        if (prev) prev->next = p->next;
                        else pf->tasks = p->next;
                        p->next = pt->tasks;
                        pt->tasks = p;
                        break;
                    }
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "DONETASK")) {
            scanf("%s %s", name, title);
            Proj *p = find_proj(name);
            if (p) {
                for (Task *t = p->tasks; t; t = t->next) {
                    if (!strcmp(t->title, title)) {
                        t->done = 1;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETEPROJECT")) {
            scanf("%s", name);
            Proj *p = head;
            Proj *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    Task *t = p->tasks;
                    while (t) {
                        Task *tmp = t;
                        t = t->next;
                        free(tmp->title);
                        free(tmp->note);
                        free(tmp);
                    }
                    free(p->name);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (Proj *p = head; p; p = p->next) {
                printf("%s\n", p->name);
                for (Task *t = p->tasks; t; t = t->next) {
                    printf("  %s %s %s\n", t->title, t->done ? "DONE" : "TODO", t->note);
                }
            }
        }
    }
    return 0;
}