/* F013.c */
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

Task *find_task(char *title, Task **list, Task **prev) {
    Task *cur = *list;
    *prev = NULL;
    while (cur) {
        if (strcmp(cur->title, title) == 0) return cur;
        *prev = cur;
        cur = cur->next;
    }
    return NULL;
}

void remove_from_list(Task **list, char *title) {
    Task *prev = NULL;
    Task *cur = *list;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else *list = cur->next;
            free(cur->title);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void add_to_list(Task **list, char *title, char *note) {
    Task *new = malloc(sizeof(Task));
    new->title = strdup(title);
    new->note = strdup(note);
    new->next = *list;
    *list = new;
}

void todo_task(char *title, char *note) {
    add_to_list(&todo, title, note);
}

void start_task(char *title) {
    // find in todo, remove, add to doing
    Task *prev;
    Task *t = find_task(title, &todo, &prev);
    if (t) {
        char *note = strdup(t->note);
        remove_from_list(&todo, title);
        add_to_list(&doing, title, note);
        free(note);
    }
}

void done_task(char *title) {
    Task *prev;
    Task *t = find_task(title, &doing, &prev);
    if (!t) {
        t = find_task(title, &todo, &prev);
    }
    if (t) {
        char *note = strdup(t->note);
        // remove from whichever list
        if (find_task(title, &doing, &prev)) {
            remove_from_list(&doing, title);
        } else if (find_task(title, &todo, &prev)) {
            remove_from_list(&todo, title);
        }
        add_to_list(&done, title, note);
        free(note);
    }
}

void remove_task(char *title) {
    // remove from all lists
    if (find_task(title, &todo, &(Task *){0})) remove_from_list(&todo, title);
    if (find_task(title, &doing, &(Task *){0})) remove_from_list(&doing, title);
    if (find_task(title, &done, &(Task *){0})) remove_from_list(&done, title);
}

void find_task_print(char *title) {
    Task *t;
    if ((t = find_task(title, &todo, &(Task *){0}))) printf("todo %s\n", t->note);
    else if ((t = find_task(title, &doing, &(Task *){0}))) printf("doing %s\n", t->note);
    else if ((t = find_task(title, &done, &(Task *){0}))) printf("done %s\n", t->note);
}

void report_all(void) {
    printf("todo:\n");
    for (Task *t = todo; t; t = t->next) printf("%s %s\n", t->title, t->note);
    printf("doing:\n");
    for (Task *t = doing; t; t = t->next) printf("%s %s\n", t->title, t->note);
    printf("done:\n");
    for (Task *t = done; t; t = t->next) printf("%s %s\n", t->title, t->note);
}

void free_list(Task *list) {
    while (list) {
        Task *next = list->next;
        free(list->title);
        free(list->note);
        free(list);
        list = next;
    }
}

void free_all(void) {
    free_list(todo);
    free_list(doing);
    free_list(done);
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "TODO") == 0 && n == 3) {
            todo_task(arg1, arg2);
        } else if (strcmp(cmd, "START") == 0 && n == 2) {
            start_task(arg1);
        } else if (strcmp(cmd, "DONE") == 0 && n == 2) {
            done_task(arg1);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 2) {
            remove_task(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_task_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}