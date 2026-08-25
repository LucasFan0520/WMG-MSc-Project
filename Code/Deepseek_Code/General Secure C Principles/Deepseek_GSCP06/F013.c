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

Task *find_task(const char *title, Task **list_ptr) {
    Task *lists[] = {todo, doing, done};
    for (int i = 0; i < 3; i++) {
        Task *cur = lists[i];
        while (cur) {
            if (strcmp(cur->title, title) == 0) {
                if (list_ptr) *list_ptr = lists[i]; // not exact, but we can return the list head
                return cur;
            }
            cur = cur->next;
        }
    }
    return NULL;
}

void remove_from_list(Task **head, const char *title) {
    Task *cur = *head, *prev = NULL;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else *head = cur->next;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void add_task(Task **head, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *head;
    *head = t;
}

void todo_cmd(const char *title, const char *note) {
    if (find_task(title, NULL)) return;
    add_task(&todo, title, note);
}

void start_task(const char *title) {
    Task *t = find_task(title, NULL);
    if (!t) return;
    // remove from current list
    if (todo) remove_from_list(&todo, title);
    else if (doing) remove_from_list(&doing, title);
    else if (done) remove_from_list(&done, title);
    // add to doing
    add_task(&doing, t->title, t->note);
    // free old task node (was removed, but we have t still)
    free(t->title);
    free(t->note);
    free(t);
}

void done_task(const char *title) {
    Task *t = find_task(title, NULL);
    if (!t) return;
    if (todo) remove_from_list(&todo, title);
    else if (doing) remove_from_list(&doing, title);
    else if (done) remove_from_list(&done, title);
    add_task(&done, t->title, t->note);
    free(t->title);
    free(t->note);
    free(t);
}

void remove_task(const char *title) {
    Task *t = find_task(title, NULL);
    if (!t) return;
    if (todo) remove_from_list(&todo, title);
    else if (doing) remove_from_list(&doing, title);
    else if (done) remove_from_list(&done, title);
    free(t->title);
    free(t->note);
    free(t);
}

void find_task_cmd(const char *title) {
    Task *t = find_task(title, NULL);
    if (!t) return;
    char *listname = "todo";
    if (todo && find_task(title, NULL) == t) listname = "todo";
    else if (doing && find_task(title, NULL) == t) listname = "doing";
    else if (done && find_task(title, NULL) == t) listname = "done";
    printf("%s %s\n", listname, t->note);
}

void report_all(void) {
    Task *lists[] = {todo, doing, done};
    char *names[] = {"todo", "doing", "done"};
    for (int i = 0; i < 3; i++) {
        Task *cur = lists[i];
        while (cur) {
            printf("%s %s %s\n", names[i], cur->title, cur->note);
            cur = cur->next;
        }
    }
}

void cleanup_tasks(void) {
    Task *cur;
    while (todo) { cur = todo; todo = todo->next; free(cur->title); free(cur->note); free(cur); }
    while (doing) { cur = doing; doing = doing->next; free(cur->title); free(cur->note); free(cur); }
    while (done) { cur = done; done = done->next; free(cur->title); free(cur->note); free(cur); }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256], rest[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, title, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "TODO") == 0) {
            char *note = line + strlen(cmd) + 1 + strlen(title) + 1;
            todo_cmd(title, note);
        } else if (strcmp(cmd, "START") == 0) {
            start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_task_cmd(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    cleanup_tasks();
    return 0;
}