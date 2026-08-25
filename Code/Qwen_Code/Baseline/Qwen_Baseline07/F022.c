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

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

Proj *find_proj(const char *name) {
    for (Proj *p = head; p; p = p->next) if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

Task *find_task(Proj *p, const char *title) {
    for (Task *t = p->tasks; t; t = t->next) if (strcmp(t->title, title) == 0) return t;
    return NULL;
}

int main() {
    char cmd[20], p1[256], p2[256], title[256], note[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "PROJECT") == 0) {
            scanf("%s", p1);
            Proj *p = malloc(sizeof(Proj));
            p->name = strdup(p1); p->tasks = NULL; p->next = head; head = p;
        } else if (strcmp(cmd, "TASK") == 0) {
            scanf("%s", p1); scanf("%s", title); read_rest(note, sizeof(note));
            Proj *p = find_proj(p1);
            if (p) {
                Task *t = malloc(sizeof(Task));
                t->title = strdup(title); t->note = strdup(note); t->done = 0; t->next = p->tasks; p->tasks = t;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            scanf("%s %s", p1, p2); scanf("%s", title);
            Proj *src = find_proj(p1), *dst = find_proj(p2);
            if (src && dst) {
                Task *prev = NULL, *curr = src->tasks;
                while (curr) {
                    if (strcmp(curr->title, title) == 0) {
                        if (prev) prev->next = curr->next; else src->tasks = curr->next;
                        curr->next = dst->tasks; dst->tasks = curr; break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            scanf("%s", p1); scanf("%s", title);
            Proj *p = find_proj(p1);
            if (p) {
                Task *t = find_task(p, title);
                if (t) t->done = 1;
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            scanf("%s", p1);
            Proj *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, p1) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    while (curr->tasks) { Task *nt = curr->tasks->next; free(curr->tasks->title); free(curr->tasks->note); free(curr->tasks); curr->tasks = nt; }
                    free(curr->name); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Proj *p = head; p; p = p->next) {
                printf("%s:\n", p->name);
                for (Task *t = p->tasks; t; t = t->next) printf("  %s %s %s\n", t->title, t->done ? "done" : "pending", t->note);
            }
        }
    }
    return 0;
}