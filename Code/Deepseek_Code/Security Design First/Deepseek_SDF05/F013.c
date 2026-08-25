// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

Task *todo = NULL;
Task *doing = NULL;
Task *done = NULL;

Task *find_task(Task *list, const char *title) {
    while (list) {
        if (strcmp(list->title, title) == 0) return list;
        list = list->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void remove_from_list(Task **list, const char *title) {
    Task *prev = NULL, *cur = *list;
    while (cur) {
        if (strcmp(cur->title, title) == 0) {
            if (prev) prev->next = cur->next;
            else *list = cur->next;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void add_to_list(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = safe_strdup(title);
    t->note = safe_strdup(note);
    t->next = *list;
    *list = t;
}

void move_task(Task **from, Task **to, const char *title) {
    Task *found = find_task(*from, title);
    if (!found) return;
    remove_from_list(from, title);
    add_to_list(to, found->title, found->note);
    free(found->title);
    free(found->note);
    free(found);
}

void todo_task(const char *title, const char *note) {
    if (find_task(todo, title) || find_task(doing, title) || find_task(done, title)) return;
    add_to_list(&todo, title, note);
}

void start_task(const char *title) {
    if (find_task(todo, title)) move_task(&todo, &doing, title);
}

void done_task(const char *title) {
    if (find_task(doing, title)) move_task(&doing, &done, title);
    else if (find_task(todo, title)) move_task(&todo, &done, title);
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

void report_all(void) {
    Task *t;
    printf("TODO:\n");
    for (t = todo; t; t = t->next) printf("%s %s\n", t->title, t->note);
    printf("DOING:\n");
    for (t = doing; t; t = t->next) printf("%s %s\n", t->title, t->note);
    printf("DONE:\n");
    for (t = done; t; t = t->next) printf("%s %s\n", t->title, t->note);
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

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "TODO") == 0) {
            if (n < 2) continue;
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            todo_task(arg1, p);
        } else if (strcmp(cmd, "START") == 0 && n >= 2) {
            start_task(arg1);
        } else if (strcmp(cmd, "DONE") == 0 && n >= 2) {
            done_task(arg1);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_task(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_task_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}