// filename: F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

static Task *todo = NULL;
static Task *doing = NULL;
static Task *done = NULL;

static void free_task(Task *t) {
    free(t->title);
    free(t->note);
    free(t);
}

static void cleanup(void) {
    while (todo) { Task *tmp = todo; todo = todo->next; free_task(tmp); }
    while (doing) { Task *tmp = doing; doing = doing->next; free_task(tmp); }
    while (done) { Task *tmp = done; done = done->next; free_task(tmp); }
}

static Task *find_task(const char *title, Task *list, Task **prev) {
    *prev = NULL;
    for (Task *t = list; t; t = t->next) {
        if (strcmp(t->title, title) == 0)
            return t;
        *prev = t;
    }
    return NULL;
}

static Task *find_in_any(const char *title, Task **list_out, Task **prev_out) {
    Task *p;
    Task *t = find_task(title, todo, &p);
    if (t) { *list_out = todo; *prev_out = p; return t; }
    t = find_task(title, doing, &p);
    if (t) { *list_out = doing; *prev_out = p; return t; }
    t = find_task(title, done, &p);
    if (t) { *list_out = done; *prev_out = p; return t; }
    return NULL;
}

static void remove_from_list(Task **head, Task *prev, Task *t) {
    if (prev) prev->next = t->next;
    else *head = t->next;
    t->next = NULL;
}

static void todo_add(const char *title, const char *note) {
    if (find_in_any(title, NULL, NULL)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    if (!t->title || !t->note) {
        free(t->title);
        free(t->note);
        free(t);
        return;
    }
    t->next = todo;
    todo = t;
}

static void start_task(const char *title) {
    Task *list, *prev;
    Task *t = find_in_any(title, &list, &prev);
    if (!t) return;
    remove_from_list(&list, prev, t);
    t->next = doing;
    doing = t;
}

static void done_task(const char *title) {
    Task *list, *prev;
    Task *t = find_in_any(title, &list, &prev);
    if (!t) return;
    remove_from_list(&list, prev, t);
    t->next = done;
    done = t;
}

static void remove_task(const char *title) {
    Task *list, *prev;
    Task *t = find_in_any(title, &list, &prev);
    if (!t) return;
    remove_from_list(&list, prev, t);
    free_task(t);
}

static void find_print(const char *title) {
    Task *list, *prev;
    Task *t = find_in_any(title, &list, &prev);
    if (!t) return;
    const char *listname = (list == todo) ? "todo" : (list == doing) ? "doing" : "done";
    printf("%s %s\n", listname, t->note);
}

static void report(void) {
    printf("todo:");
    for (Task *t = todo; t; t = t->next) printf(" %s", t->title);
    putchar('\n');
    printf("doing:");
    for (Task *t = doing; t; t = t->next) printf(" %s", t->title);
    putchar('\n');
    printf("done:");
    for (Task *t = done; t; t = t->next) printf(" %s", t->title);
    putchar('\n');
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "TODO") == 0) {
            if (*arg1) todo_add(arg1, arg2);
        } else if (strcmp(cmd, "START") == 0) {
            if (*arg1) start_task(arg1);
        } else if (strcmp(cmd, "DONE") == 0) {
            if (*arg1) done_task(arg1);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (*arg1) remove_task(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free(line);
    return 0;
}