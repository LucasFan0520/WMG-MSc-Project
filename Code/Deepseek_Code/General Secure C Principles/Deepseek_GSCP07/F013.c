// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

typedef struct List {
    char *name;
    Task *tasks;
} List;

List todo = {"TODO", NULL};
List doing = {"DOING", NULL};
List done = {"DONE", NULL};

Task* find_task_in_list(List *list, const char *title) {
    Task *t = list->tasks;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

Task* find_task_any(const char *title, List **which) {
    Task *t;
    t = find_task_in_list(&todo, title); if (t) { *which = &todo; return t; }
    t = find_task_in_list(&doing, title); if (t) { *which = &doing; return t; }
    t = find_task_in_list(&done, title); if (t) { *which = &done; return t; }
    return NULL;
}

void free_task(Task *t) {
    free(t->title);
    free(t->note);
    free(t);
}

void remove_task_from_list(List *list, const char *title) {
    Task *t = list->tasks, *prev = NULL;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else list->tasks = t->next;
            free_task(t);
            return;
        }
        prev = t;
        t = t->next;
    }
}

void add_task_to_list(List *list, const char *title, const char *note) {
    if (find_task_in_list(list, title)) return;
    Task *new = malloc(sizeof(Task));
    new->title = strdup(title);
    new->note = strdup(note);
    new->next = NULL;
    if (!list->tasks) list->tasks = new;
    else {
        Task *last = list->tasks;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void todo_cmd(const char *title, const char *note) {
    // Remove from any list first
    List *which;
    Task *t = find_task_any(title, &which);
    if (t) {
        remove_task_from_list(which, title);
    }
    add_task_to_list(&todo, title, note);
}

void start_cmd(const char *title) {
    List *which;
    Task *t = find_task_any(title, &which);
    if (!t) return;
    char *note = strdup(t->note);
    remove_task_from_list(which, title);
    add_task_to_list(&doing, title, note);
    free(note);
}

void done_cmd(const char *title) {
    List *which;
    Task *t = find_task_any(title, &which);
    if (!t) return;
    char *note = strdup(t->note);
    remove_task_from_list(which, title);
    add_task_to_list(&done, title, note);
    free(note);
}

void remove_cmd(const char *title) {
    List *which;
    Task *t = find_task_any(title, &which);
    if (!t) return;
    remove_task_from_list(which, title);
}

void find_cmd(const char *title) {
    List *which;
    Task *t = find_task_any(title, &which);
    if (!t) return;
    printf("%s %s\n", which->name, t->note);
}

void report() {
    List *lists[] = {&todo, &doing, &done};
    for (int i = 0; i < 3; i++) {
        printf("%s\n", lists[i]->name);
        Task *t = lists[i]->tasks;
        while (t) {
            printf("%s %s\n", t->title, t->note);
            t = t->next;
        }
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "TODO") == 0) {
            char *title = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (title) {
                if (!note) note = "";
                todo_cmd(title, note);
            }
        } else if (strcmp(cmd, "START") == 0) {
            char *title = strtok(NULL, " ");
            if (title) start_cmd(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) done_cmd(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = strtok(NULL, " ");
            if (title) remove_cmd(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_cmd(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    // Free all tasks in all lists
    List *lists[] = {&todo, &doing, &done};
    for (int i = 0; i < 3; i++) {
        while (lists[i]->tasks) {
            Task *tmp = lists[i]->tasks;
            lists[i]->tasks = lists[i]->tasks->next;
            free_task(tmp);
        }
    }
    return 0;
}