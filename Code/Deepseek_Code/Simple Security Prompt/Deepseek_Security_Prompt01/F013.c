// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *note; } Task;
typedef struct { Task *tasks; int count, cap; } List;

List todo = {NULL, 0, 0};
List doing = {NULL, 0, 0};
List done_list = {NULL, 0, 0};

int find_in_list(List *list, const char *title) {
    for (int i = 0; i < list->count; i++)
        if (strcmp(list->tasks[i].title, title) == 0) return i;
    return -1;
}

void add_to_list(List *list, const char *title, const char *note) {
    if (find_in_list(list, title) >= 0) return;
    if (list->count == list->cap) {
        list->cap = list->cap ? list->cap * 2 : 4;
        list->tasks = realloc(list->tasks, sizeof(Task) * list->cap);
    }
    list->tasks[list->count].title = strdup(title);
    list->tasks[list->count].note = strdup(note);
    list->count++;
}

void remove_from_list(List *list, const char *title) {
    int idx = find_in_list(list, title);
    if (idx < 0) return;
    free(list->tasks[idx].title);
    free(list->tasks[idx].note);
    for (int i = idx; i < list->count - 1; i++) list->tasks[i] = list->tasks[i + 1];
    list->count--;
}

void move_task(List *from, List *to, const char *title) {
    int idx = find_in_list(from, title);
    if (idx < 0) return;
    char *note = strdup(from->tasks[idx].note);
    remove_from_list(from, title);
    add_to_list(to, title, note);
    free(note);
}

void add_todo(const char *title, const char *note) {
    add_to_list(&todo, title, note);
}

void start_task(const char *title) {
    move_task(&todo, &doing, title);
}

void done_task(const char *title) {
    move_task(&doing, &done_list, title);
}

void remove_task(const char *title) {
    if (find_in_list(&todo, title) >= 0) remove_from_list(&todo, title);
    else if (find_in_list(&doing, title) >= 0) remove_from_list(&doing, title);
    else if (find_in_list(&done_list, title) >= 0) remove_from_list(&done_list, title);
}

void find_task(const char *title) {
    List *lists[3] = {&todo, &doing, &done_list};
    const char *names[3] = {"TODO", "DOING", "DONE"};
    for (int i = 0; i < 3; i++) {
        int idx = find_in_list(lists[i], title);
        if (idx >= 0) {
            printf("%s %s\n", names[i], lists[i]->tasks[idx].note);
            return;
        }
    }
}

void report_all(void) {
    List *lists[3] = {&todo, &doing, &done_list};
    const char *names[3] = {"TODO", "DOING", "DONE"};
    for (int i = 0; i < 3; i++) {
        printf("%s\n", names[i]);
        for (int j = 0; j < lists[i]->count; j++)
            printf("%s %s\n", lists[i]->tasks[j].title, lists[i]->tasks[j].note);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256], note[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "TODO") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", title, note) == 2)
                add_todo(title, note);
        } else if (strcmp(cmd, "START") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) find_task(title);
        } else if (strcmp(cmd, "REPORT") == 0) report_all();
    }
    for (int i = 0; i < todo.count; i++) { free(todo.tasks[i].title); free(todo.tasks[i].note); }
    for (int i = 0; i < doing.count; i++) { free(doing.tasks[i].title); free(doing.tasks[i].note); }
    for (int i = 0; i < done_list.count; i++) { free(done_list.tasks[i].title); free(done_list.tasks[i].note); }
    free(todo.tasks);
    free(doing.tasks);
    free(done_list.tasks);
    return 0;
}