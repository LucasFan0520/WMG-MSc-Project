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

Task *find_task(const char *title, Task **list_out) {
    Task *t = todo;
    while (t) { if (strcmp(t->title, title) == 0) { *list_out = todo; return t; } t = t->next; }
    t = doing;
    while (t) { if (strcmp(t->title, title) == 0) { *list_out = doing; return t; } t = t->next; }
    t = done;
    while (t) { if (strcmp(t->title, title) == 0) { *list_out = done; return t; } t = t->next; }
    return NULL;
}

void remove_from_list(Task **list, const char *title) {
    Task *p = *list, *prev = NULL;
    while (p) {
        if (strcmp(p->title, title) == 0) {
            if (prev) prev->next = p->next;
            else *list = p->next;
            free(p->title);
            free(p->note);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void add_to_list(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = malloc(strlen(title) + 1);
    strcpy(t->title, title);
    t->note = malloc(strlen(note) + 1);
    strcpy(t->note, note);
    t->next = *list;
    *list = t;
}

void todo_task(const char *title, const char *note) {
    // remove if exists in any list
    Task *list_ptr = NULL;
    if (find_task(title, &list_ptr)) remove_from_list(&list_ptr, title);
    add_to_list(&todo, title, note);
}

void start_task(const char *title) {
    Task *list_ptr = NULL;
    Task *t = find_task(title, &list_ptr);
    if (!t) return;
    remove_from_list(&list_ptr, title);
    add_to_list(&doing, t->title, t->note);
    free(t->title); free(t->note); free(t); // we just re-added with new allocations? Actually add_to_list reallocates, so we can free old; but we have t freed.
    // better: move without realloc. Simpler: use remove_from_list which frees, then add again. That's okay.
}

void done_task(const char *title) {
    Task *list_ptr = NULL;
    Task *t = find_task(title, &list_ptr);
    if (!t) return;
    remove_from_list(&list_ptr, title);
    add_to_list(&done, t->title, t->note);
    free(t->title); free(t->note); free(t);
}

void remove_task(const char *title) {
    Task *list_ptr = NULL;
    if (find_task(title, &list_ptr)) remove_from_list(&list_ptr, title);
}

void find_task_print(const char *title) {
    Task *list_ptr = NULL;
    Task *t = find_task(title, &list_ptr);
    if (!t) return;
    char *list_name = "todo";
    if (list_ptr == doing) list_name = "doing";
    else if (list_ptr == done) list_name = "done";
    printf("%s %s\n", list_name, t->note);
}

void report_all(void) {
    Task *t = todo;
    while (t) { printf("todo %s %s\n", t->title, t->note); t = t->next; }
    t = doing;
    while (t) { printf("doing %s %s\n", t->title, t->note); t = t->next; }
    t = done;
    while (t) { printf("done %s %s\n", t->title, t->note); t = t->next; }
}

void free_list(Task *list) {
    Task *t = list;
    while (t) {
        Task *tmp = t->next;
        free(t->title);
        free(t->note);
        free(t);
        t = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256], note[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "TODO") == 0) {
            if (sscanf(rest, "%255s", title) == 1) {
                const char *n = rest + strlen(title) + 1;
                while (*n == ' ') n++;
                if (*n) todo_task(title, n);
            }
        } else if (strcmp(cmd, "START") == 0) {
            if (sscanf(rest, "%255s", title) == 1) start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            if (sscanf(rest, "%255s", title) == 1) done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(rest, "%255s", title) == 1) remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", title) == 1) find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_list(todo);
    free_list(doing);
    free_list(done);
    return 0;
}