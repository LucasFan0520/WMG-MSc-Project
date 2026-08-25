// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

Task *todo = NULL;
Task *doing = NULL;
Task *done = NULL;

void add_task(Task **list, const char *t, const char *n) {
    Task *p = malloc(sizeof(Task));
    p->title = strdup(t);
    p->note = strdup(n);
    p->next = *list;
    *list = p;
}

void move_task(Task **from, Task **to, const char *t) {
    Task *p = *from;
    Task *prev = NULL;
    while (p) {
        if (strcmp(p->title, t) == 0) {
            Task *next = p->next;
            if (prev) {
                prev->next = next;
            } else {
                *from = next;
            }
            p->next = *to;
            *to = p;
            return;
        }
        prev = p;
        p = p->next;
    }
}

void remove_task(const char *t) {
    Task **refs[3] = {&todo, &doing, &done};
    for (int i = 0; i < 3; i++) {
        Task *p = *refs[i];
        Task *prev = NULL;
        while (p) {
            if (strcmp(p->title, t) == 0) {
                Task *next = p->next;
                if (prev) {
                    prev->next = next;
                } else {
                    *refs[i] = next;
                }
                free(p->title);
                free(p->note);
                free(p);
                return;
            }
            prev = p;
            p = p->next;
        }
    }
}

void find_task(const char *t) {
    Task *lists[3] = {todo, doing, done};
    char *names[3] = {"todo", "doing", "done"};
    for (int i = 0; i < 3; i++) {
        for (Task *p = lists[i]; p; p = p->next) {
            if (strcmp(p->title, t) == 0) {
                printf("%s %s\n", names[i], p->note);
                return;
            }
        }
    }
}

void report_tasks() {
    char *names[3] = {"todo", "doing", "done"};
    Task *lists[3] = {todo, doing, done};
    for (int i = 0; i < 3; i++) {
        printf("%s:\n", names[i]);
        for (Task *p = lists[i]; p; p = p->next) {
            printf("  %s %s\n", p->title, p->note);
        }
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char t[1000];
        char n[8000];
        n[0] = 0;
        sscanf(line, "%s %s %[^\n]", cmd, t, n);
        if (strcmp(cmd, "TODO") == 0) {
            add_task(&todo, t, n);
        } else if (strcmp(cmd, "START") == 0) {
            move_task(&todo, &doing, t);
        } else if (strcmp(cmd, "DONE") == 0) {
            move_task(&doing, &done, t);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_task(t);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_task(t);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_tasks();
        }
    }
    return 0;
}