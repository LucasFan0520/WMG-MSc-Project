// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Task { char *title, *note; struct Task *next; } Task;
Task *todo = NULL, *doing = NULL, *done = NULL;

void add_task(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = *list;
    *list = t;
}

void move_task(Task **from, Task **to, const char *title) {
    Task **p = from;
    while (*p) {
        if (strcmp((*p)->title, title) == 0) {
            Task *tmp = *p;
            *p = tmp->next;
            tmp->next = *to;
            *to = tmp;
            return;
        }
        p = &(*p)->next;
    }
}

void remove_task(const char *title) {
    Task **heads[] = {&todo, &doing, &done};
    for (int i = 0; i < 3; i++) {
        Task **p = heads[i];
        while (*p) {
            if (strcmp((*p)->title, title) == 0) {
                Task *tmp = *p;
                *p = tmp->next;
                free(tmp->title);
                free(tmp->note);
                free(tmp);
                return;
            }
            p = &(*p)->next;
        }
    }
}

void find_task(const char *title) {
    Task *lists[] = {todo, doing, done};
    char *names[] = {"todo", "doing", "done"};
    for (int i = 0; i < 3; i++) {
        for (Task *t = lists[i]; t; t = t->next) {
            if (strcmp(t->title, title) == 0) { printf("%s %s\n", names[i], t->note); return; }
        }
    }
}

void report() {
    char *names[] = {"todo", "doing", "done"};
    Task *lists[] = {todo, doing, done};
    for (int i = 0; i < 3; i++) {
        printf("%s:\n", names[i]);
        for (Task *t = lists[i]; t; t = t->next) printf("  %s %s\n", t->title, t->note);
    }
}

void cleanup_list(Task *list) {
    while (list) {
        Task *tmp = list->next;
        free(list->title);
        free(list->note);
        free(list);
        list = tmp;
    }
}

void cleanup() {
    cleanup_list(todo);
    cleanup_list(doing);
    cleanup_list(done);
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256], note[8192] = "";
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "TODO") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) strcpy(note, t + 1); }
                add_task(&todo, title, note);
            }
        } else if (strcmp(cmd, "START") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) move_task(&todo, &doing, title);
        } else if (strcmp(cmd, "DONE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) move_task(&doing, &done, title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1) find_task(title);
        } else if (strcmp(cmd, "REPORT") == 0) report();
    }
    cleanup();
    return 0;
}