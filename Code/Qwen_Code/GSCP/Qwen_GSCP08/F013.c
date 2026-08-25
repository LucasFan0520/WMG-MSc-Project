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

Task *find_task(Task *list, char *title) {
    for (Task *t = list; t; t = t->next) {
        if (strcmp(t->title, title) == 0) return t;
    }
    return NULL;
}

void add_task(Task **list, char *title, char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void remove_task(Task **list, char *title) {
    Task *prev = NULL;
    Task *curr = *list;
    while (curr) {
        if (strcmp(curr->title, title) == 0) {
            if (prev) prev->next = curr->next;
            else *list = curr->next;
            free(curr->title);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void move_task(Task **from, Task **to, char *title) {
    Task *t = find_task(*from, title);
    if (t) {
        add_task(to, t->title, t->note);
        remove_task(from, title);
    }
}

void find_task_info(char *title) {
    Task *t = find_task(todo, title);
    if (t) { printf("todo %s\n", t->note); return; }
    t = find_task(doing, title);
    if (t) { printf("doing %s\n", t->note); return; }
    t = find_task(done, title);
    if (t) { printf("done %s\n", t->note); return; }
}

void report_tasks() {
    printf("TODO:");
    for (Task *t = todo; t; t = t->next) printf(" %s", t->title);
    puts("");
    printf("DOING:");
    for (Task *t = doing; t; t = t->next) printf(" %s", t->title);
    puts("");
    printf("DONE:");
    for (Task *t = done; t; t = t->next) printf(" %s", t->title);
    puts("");
}

int main() {
    char cmd[20], title[100], note[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "TODO") == 0) {
            scanf("%99s", title);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            add_task(&todo, title, note);
        } else if (strcmp(cmd, "START") == 0) {
            scanf("%99s", title);
            move_task(&todo, &doing, title);
        } else if (strcmp(cmd, "DONE") == 0) {
            scanf("%99s", title);
            if (find_task(todo, title)) move_task(&todo, &done, title);
            else move_task(&doing, &done, title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%99s", title);
            remove_task(&todo, title);
            remove_task(&doing, title);
            remove_task(&done, title);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", title);
            find_task_info(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_tasks();
        }
    }
    return 0;
}