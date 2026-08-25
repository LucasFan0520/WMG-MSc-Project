// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task {
    char *title;
    char *note;
    char *list;
    struct Task *next;
} Task;

Task *head = NULL;

Task *find_task(const char *title) {
    for (Task *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->title, title) == 0) return curr;
    }
    return NULL;
}

void add_task(const char *list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->list = strdup(list);
    t->next = head;
    head = t;
}

void move_task(const char *title, const char *new_list) {
    Task *t = find_task(title);
    if (t) {
        free(t->list);
        t->list = strdup(new_list);
    }
}

void remove_task(const char *title) {
    Task *curr = head;
    Task *prev = NULL;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            Task *next = curr->next;
            free(curr->title);
            free(curr->note);
            free(curr->list);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_task_print(const char *title) {
    Task *t = find_task(title);
    if (t) {
        printf("%s %s\n", t->list, t->note);
    }
}

void report_tasks() {
    printf("todo:\n");
    for (Task *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->list, "todo") == 0) printf("%s %s\n", curr->title, curr->note);
    }
    printf("doing:\n");
    for (Task *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->list, "doing") == 0) printf("%s %s\n", curr->title, curr->note);
    }
    printf("done:\n");
    for (Task *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->list, "done") == 0) printf("%s %s\n", curr->title, curr->note);
    }
}

void free_all() {
    Task *curr = head;
    while (curr) {
        Task *next = curr->next;
        free(curr->title);
        free(curr->note);
        free(curr->list);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char title[1024], note[4096];
        
        int n = sscanf(line, "%15s %1023s %[^\n]", cmd, title, note);
        if (strcmp(cmd, "TODO") == 0 && n == 3) {
            add_task("todo", title, note);
        } else if (strcmp(cmd, "START") == 0 && n >= 2) {
            move_task(title, "doing");
        } else if (strcmp(cmd, "DONE") == 0 && n >= 2) {
            move_task(title, "done");
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_tasks();
        }
    }
    free_all();
    return 0;
}