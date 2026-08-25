// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct task {
    char *title;
    char *note;
    struct task *next;
} Task;

Task *todo = NULL, *doing = NULL, *done = NULL;

Task *find_in_list(Task *list, const char *title) {
    while (list) {
        if (strcmp(list->title, title) == 0) return list;
        list = list->next;
    }
    return NULL;
}

void add_task(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void remove_from_list(Task **list, const char *title) {
    Task *t = *list, *prev = NULL;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else *list = t->next;
            free(t->title);
            free(t->note);
            free(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

Task *find_task(const char *title) {
    Task *t = find_in_list(todo, title);
    if (t) return t;
    t = find_in_list(doing, title);
    if (t) return t;
    return find_in_list(done, title);
}

void todo_task(const char *title, const char *note) {
    if (find_task(title)) return;
    add_task(&todo, title, note);
}

void start_task(const char *title) {
    Task *t = find_in_list(todo, title);
    if (!t) return;
    remove_from_list(&todo, title);
    add_task(&doing, title, t->note);
    free(t->title);
    free(t->note);
    free(t);
}

void done_task(const char *title) {
    Task *t = find_in_list(doing, title);
    if (!t) return;
    remove_from_list(&doing, title);
    add_task(&done, title, t->note);
    free(t->title);
    free(t->note);
    free(t);
}

void remove_task(const char *title) {
    if (find_in_list(todo, title)) remove_from_list(&todo, title);
    else if (find_in_list(doing, title)) remove_from_list(&doing, title);
    else if (find_in_list(done, title)) remove_from_list(&done, title);
}

void find_task_print(const char *title) {
    Task *t = find_task(title);
    if (!t) return;
    if (find_in_list(todo, title)) printf("todo %s\n", t->note);
    else if (find_in_list(doing, title)) printf("doing %s\n", t->note);
    else if (find_in_list(done, title)) printf("done %s\n", t->note);
}

void report_lists(void) {
    printf("todo\n");
    Task *t = todo;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    printf("doing\n");
    t = doing;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    printf("done\n");
    t = done;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256], note[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "TODO") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", title, note) == 2)
                todo_task(title, note);
        } else if (strcmp(cmd, "START") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_lists();
        }
    }
    return 0;
}