// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

Task *todo = NULL, *doing = NULL, *done = NULL;

Task *find_task(Task **list, const char *title) {
    Task *curr = *list;
    while (curr) {
        if (strcmp(curr->title, title) == 0) return curr;
        curr = curr->next;
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

void move_task(Task **from, Task **to, const char *title) {
    Task *curr = *from, *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else *from = curr->next;
            curr->next = *to;
            *to = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void remove_task(const char *title) {
    Task *lists[3] = {todo, doing, done};
    Task **ptrs[3] = {&todo, &doing, &done};
    for (int i = 0; i < 3; i++) {
        Task *curr = *ptrs[i], *prev = NULL;
        while (curr) {
            if (strcmp(curr->title, title) == 0) {
                if (prev) prev->next = curr->next;
                else *ptrs[i] = curr->next;
                free(curr->title);
                free(curr->note);
                free(curr);
                return;
            }
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_task_print(const char *title) {
    Task *lists[3] = {todo, doing, done};
    char *names[3] = {"todo", "doing", "done"};
    for (int i = 0; i < 3; i++) {
        Task *curr = lists[i];
        while (curr) {
            if (strcmp(curr->title, title) == 0) {
                printf("%s %s\n", names[i], curr->note);
                return;
            }
            curr = curr->next;
        }
    }
}

void print_list(Task *list) {
    Task *curr = list;
    while (curr) {
        printf("%s %s\n", curr->title, curr->note);
        curr = curr->next;
    }
}

void report_tasks() {
    printf("todo:\n"); print_list(todo);
    printf("doing:\n"); print_list(doing);
    printf("done:\n"); print_list(done);
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], title[256], note[4096] = {0};
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, title, note);
        
        if (strcmp(cmd, "TODO") == 0 && n >= 2) add_task(&todo, title, note);
        else if (strcmp(cmd, "START") == 0 && n >= 1) move_task(&todo, &doing, title);
        else if (strcmp(cmd, "DONE") == 0 && n >= 1) move_task(&doing, &done, title);
        else if (strcmp(cmd, "REMOVE") == 0 && n >= 1) remove_task(title);
        else if (strcmp(cmd, "FIND") == 0 && n >= 1) find_task_print(title);
        else if (strcmp(cmd, "REPORT") == 0) report_tasks();
    }
    free_list(todo); free_list(doing); free_list(done);
    return 0;
}