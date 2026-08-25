/*
 * F013.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct task {
    char *title;
    char *note;
    struct task *next;
} Task;

Task *todo = NULL, *doing = NULL, *done = NULL;

Task *find_task(Task *list, const char *title) {
    while (list) {
        if (strcmp(list->title, title) == 0) return list;
        list = list->next;
    }
    return NULL;
}

void remove_from_list(Task **list, const char *title) {
    Task *prev = NULL, *t = *list;
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

void add_task(Task **list, const char *title, const char *note) {
    if (find_task(*list, title)) return;
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void todo_add(const char *title, const char *note) {
    add_task(&todo, title, note);
}

void start_task(const char *title) {
    Task *t = find_task(todo, title);
    if (t) {
        remove_from_list(&todo, title);
        add_task(&doing, title, t->note); // but we need to preserve note
        // we removed t and lost note; better to extract note before removal
        // redo: find and remove with note copy
        Task *found = find_task(todo, title);
        if (found) {
            char *note = strdup(found->note);
            remove_from_list(&todo, title);
            add_task(&doing, title, note);
            free(note);
        }
    }
}

void done_task(const char *title) {
    Task *t = find_task(todo, title);
    if (t) {
        char *note = strdup(t->note);
        remove_from_list(&todo, title);
        add_task(&done, title, note);
        free(note);
        return;
    }
    t = find_task(doing, title);
    if (t) {
        char *note = strdup(t->note);
        remove_from_list(&doing, title);
        add_task(&done, title, note);
        free(note);
    }
}

void remove_task(const char *title) {
    if (find_task(todo, title)) remove_from_list(&todo, title);
    else if (find_task(doing, title)) remove_from_list(&doing, title);
    else if (find_task(done, title)) remove_from_list(&done, title);
}

void find_task_cmd(const char *title) {
    Task *t = find_task(todo, title);
    if (t) { printf("TODO %s\n", t->note); return; }
    t = find_task(doing, title);
    if (t) { printf("DOING %s\n", t->note); return; }
    t = find_task(done, title);
    if (t) { printf("DONE %s\n", t->note); return; }
    printf("NOT_FOUND\n");
}

void report_lists(void) {
    printf("TODO:\n");
    Task *t = todo;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    printf("DOING:\n");
    t = doing;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    printf("DONE:\n");
    t = done;
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256];
        if (sscanf(line, "%31s %255s", cmd, title) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(title) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "TODO") == 0) {
            todo_add(title, rest);
        } else if (strcmp(cmd, "START") == 0) {
            start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_task_cmd(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_lists();
        }
    }
    return 0;
}