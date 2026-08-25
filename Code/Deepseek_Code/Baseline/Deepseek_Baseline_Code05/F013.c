/* F013.c */
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

Task *find_task(const char *title, Task **owner) {
    Task *t = find_in_list(todo, title); if (t) { *owner = todo; return t; }
    t = find_in_list(doing, title); if (t) { *owner = doing; return t; }
    t = find_in_list(done, title); if (t) { *owner = done; return t; }
    return NULL;
}

void add_todo(const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = todo;
    todo = t;
}

void move_task(Task **from, Task **to, const char *title) {
    Task *t = *from, *prev = NULL;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else *from = t->next;
            t->next = *to;
            *to = t;
            return;
        }
        prev = t;
        t = t->next;
    }
}

void start_task(const char *title) {
    Task *owner = NULL;
    Task *t = find_task(title, &owner);
    if (!t) return;
    if (owner == todo) move_task(&todo, &doing, title);
    // else ignore if not in todo
}

void done_task(const char *title) {
    Task *owner = NULL;
    Task *t = find_task(title, &owner);
    if (!t) return;
    if (owner == todo) move_task(&todo, &done, title);
    else if (owner == doing) move_task(&doing, &done, title);
    // else ignore
}

void remove_task(const char *title) {
    Task *owner = NULL;
    Task *t = find_task(title, &owner);
    if (!t) return;
    // remove from owner
    Task *prev = NULL, *cur = owner;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else owner = cur->next;
            free(cur->title); free(cur->note); free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_task_print(const char *title) {
    Task *owner = NULL;
    Task *t = find_task(title, &owner);
    if (!t) { printf("NOT_FOUND\n"); return; }
    const char *listname;
    if (owner == todo) listname = "todo";
    else if (owner == doing) listname = "doing";
    else listname = "done";
    printf("%s ", listname);
    for (char *c = t->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void print_list(Task *list, const char *name) {
    printf("%s\n", name);
    Task *t = list;
    while (t) {
        printf("%s ", t->title);
        for (char *c = t->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        t = t->next;
    }
}

void report() {
    print_list(todo, "todo");
    print_list(doing, "doing");
    print_list(done, "done");
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], title[256], note[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report(); continue; }
        if (strcmp(cmd, "TODO") == 0) {
            if (sscanf(line, "%*s %255s", title) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(title) + 1;
            if (*rest == ' ') rest++;
            add_todo(title, rest);
        } else if (strcmp(cmd, "START") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) find_task_print(title);
        }
    }
    return 0;
}