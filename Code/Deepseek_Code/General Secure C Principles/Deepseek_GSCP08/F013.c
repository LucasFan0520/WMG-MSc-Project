// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task { char *title; char *note; struct Task *next; } Task;
Task *todo = NULL, *doing = NULL, *done = NULL;
Task *tail_todo = NULL, *tail_doing = NULL, *tail_done = NULL;

Task *find_task(const char *title, Task **list, Task ***tailptr) {
    for (Task *t = *list; t; t = t->next)
        if (strcmp(t->title, title) == 0) return t;
    return NULL;
}

void add_task(Task **head, Task **tail, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title); t->note = strdup(note); t->next = NULL;
    if (*tail) (*tail)->next = t; else *head = t;
    *tail = t;
}

void move_task(Task **from_head, Task **from_tail, Task **to_head, Task **to_tail, const char *title) {
    Task *prev = NULL;
    for (Task *t = *from_head; t; prev = t, t = t->next) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next; else *from_head = t->next;
            if (t == *from_tail) *from_tail = prev;
            t->next = NULL;
            if (*to_tail) (*to_tail)->next = t; else *to_head = t;
            *to_tail = t;
            return;
        }
    }
}

void todo_task(const char *title, const char *note) {
    // If task exists anywhere, do nothing? Spec: "TODO title note adds a task to the todo list"
    // Assuming if already exists, ignore.
    if (find_task(title, &todo, &tail_todo)) return;
    if (find_task(title, &doing, &tail_doing)) return;
    if (find_task(title, &done, &tail_done)) return;
    add_task(&todo, &tail_todo, title, note);
}

void start_task(const char *title) {
    if (find_task(title, &doing, &tail_doing)) return;
    if (find_task(title, &todo, &tail_todo)) {
        move_task(&todo, &tail_todo, &doing, &tail_doing, title);
    }
}

void done_task(const char *title) {
    if (find_task(title, &done, &tail_done)) return;
    if (find_task(title, &todo, &tail_todo)) {
        move_task(&todo, &tail_todo, &done, &tail_done, title);
    } else if (find_task(title, &doing, &tail_doing)) {
        move_task(&doing, &tail_doing, &done, &tail_done, title);
    }
}

void remove_task(const char *title) {
    Task *prev = NULL;
    for (Task **head = &todo, **tail = &tail_todo; ; ) {
        for (Task *t = *head; t; prev = t, t = t->next) {
            if (strcmp(t->title, title) == 0) {
                if (prev) prev->next = t->next; else *head = t->next;
                if (t == *tail) *tail = prev;
                free(t->title); free(t->note); free(t);
                return;
            }
        }
        if (head == &todo) { head = &doing; tail = &tail_doing; prev = NULL; }
        else if (head == &doing) { head = &done; tail = &tail_done; prev = NULL; }
        else break;
    }
}

void find_task_print(const char *title) {
    Task *t = find_task(title, &todo, &tail_todo);
    if (t) { printf("todo "); for (char *c = t->note; *c; c++) putchar(*c==' '?'_':*c); putchar('\n'); return; }
    t = find_task(title, &doing, &tail_doing);
    if (t) { printf("doing "); for (char *c = t->note; *c; c++) putchar(*c==' '?'_':*c); putchar('\n'); return; }
    t = find_task(title, &done, &tail_done);
    if (t) { printf("done "); for (char *c = t->note; *c; c++) putchar(*c==' '?'_':*c); putchar('\n'); return; }
    printf("NOT_FOUND\n");
}

void report_all(void) {
    Task *t;
    printf("todo\n");
    for (t = todo; t; t = t->next) printf("%s %s\n", t->title, t->note);
    printf("doing\n");
    for (t = doing; t; t = t->next) printf("%s %s\n", t->title, t->note);
    printf("done\n");
    for (t = done; t; t = t->next) printf("%s %s\n", t->title, t->note);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "TODO") == 0) {
            char title[256], note[4096];
            if (sscanf(rest, "%255s", title) == 1) {
                char *n = rest + strlen(title);
                while (*n == ' ') n++;
                strcpy(note, n);
                todo_task(title, note);
            }
        } else if (strcmp(cmd, "START") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) == 1) find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    // cleanup all tasks
    while (todo) { Task *t = todo; todo = t->next; free(t->title); free(t->note); free(t); }
    while (doing) { Task *t = doing; doing = t->next; free(t->title); free(t->note); free(t); }
    while (done) { Task *t = done; done = t->next; free(t->title); free(t->note); free(t); }
    return 0;
}