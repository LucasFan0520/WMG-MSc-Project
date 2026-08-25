/* F013.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

Task *todo = NULL, *doing = NULL, *done = NULL;

Task* find_task(const char *title, Task **list_out) {
    Task *lists[] = {todo, doing, done};
    const char *names[] = {"todo", "doing", "done"};
    for (int i = 0; i < 3; i++) {
        Task *t = lists[i];
        while (t) {
            if (strcmp(t->title, title) == 0) {
                if (list_out) *list_out = lists[i];
                return t;
            }
            t = t->next;
        }
    }
    return NULL;
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

void add_task(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void todo_add(const char *title, const char *note) {
    if (find_task(title, NULL)) return;
    add_task(&todo, title, note);
}

void start_task(const char *title) {
    Task *t = find_task(title, NULL);
    if (!t) return;
    // remove from wherever
    remove_from_list(&todo, title);
    remove_from_list(&doing, title);
    remove_from_list(&done, title);
    // add to doing with same note
    add_task(&doing, t->title, t->note);
}

void done_task(const char *title) {
    Task *t = find_task(title, NULL);
    if (!t) return;
    remove_from_list(&todo, title);
    remove_from_list(&doing, title);
    remove_from_list(&done, title);
    add_task(&done, t->title, t->note);
}

void remove_task(const char *title) {
    remove_from_list(&todo, title);
    remove_from_list(&doing, title);
    remove_from_list(&done, title);
}

void find_task_print(const char *title) {
    Task *t = find_task(title, NULL);
    if (!t) return;
    const char *listname;
    if (find_task(title, &listname) && listname == todo) listname = "todo";
    else if (listname == doing) listname = "doing";
    else listname = "done";
    printf("%s %s\n", listname, t->note);
}

void report_all() {
    Task *t = todo;
    printf("todo:\n");
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    t = doing;
    printf("doing:\n");
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
    t = done;
    printf("done:\n");
    while (t) { printf("%s %s\n", t->title, t->note); t = t->next; }
}

void free_all() {
    while (todo) { Task *tmp = todo; todo = todo->next; free(tmp->title); free(tmp->note); free(tmp); }
    while (doing) { Task *tmp = doing; doing = doing->next; free(tmp->title); free(tmp->note); free(tmp); }
    while (done) { Task *tmp = done; done = done->next; free(tmp->title); free(tmp->note); free(tmp); }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char title[256]; char note[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, title, note);
        if (strcmp(cmd, "TODO") == 0 && n == 3) {
            todo_add(title, note);
        } else if (strcmp(cmd, "START") == 0 && n == 2) {
            start_task(title);
        } else if (strcmp(cmd, "DONE") == 0 && n == 2) {
            done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 2) {
            remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free_all();
    return 0;
}