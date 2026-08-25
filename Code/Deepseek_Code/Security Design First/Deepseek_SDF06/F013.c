// F013.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *note;
} Task;

static Task *todo = NULL, *doing = NULL, *done = NULL;
static size_t todo_count = 0, doing_count = 0, done_count = 0;
static size_t todo_cap = 0, doing_cap = 0, done_cap = 0;

static ssize_t find_task(Task *list, size_t n, const char *title) {
    for (size_t i = 0; i < n; i++)
        if (strcmp(list[i].title, title) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_task(Task **list, size_t *count, size_t *cap, const char *title, const char *note) {
    if (*count == *cap) {
        *cap = *cap ? *cap * 2 : 4;
        Task *new = realloc(*list, *cap * sizeof(Task));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        *list = new;
    }
    (*list)[*count].title = strdup(title);
    (*list)[*count].note = strdup(note);
    if (!(*list)[*count].title || !(*list)[*count].note) { fprintf(stderr, "Memory error\n"); exit(1); }
    (*count)++;
}

static void remove_task(Task **list, size_t *count, size_t idx) {
    free((*list)[idx].title);
    free((*list)[idx].note);
    for (size_t i = idx; i < *count - 1; i++)
        (*list)[i] = (*list)[i + 1];
    (*count)--;
}

static void move_task(Task **from, size_t *from_count, Task **to, size_t *to_count,
                      size_t *from_cap, size_t *to_cap,
                      const char *title, const char *note) {
    ssize_t idx = find_task(*from, *from_count, title);
    if (idx == -1) return;
    // add to destination
    add_task(to, to_count, to_cap, (*from)[idx].title, note ? note : (*from)[idx].note);
    // remove from source
    remove_task(from, from_count, idx);
}

static void todo_add(const char *title, const char *note) {
    add_task(&todo, &todo_count, &todo_cap, title, note);
}

static void start_task(const char *title) {
    move_task(&todo, &todo_count, &doing, &doing_count, &todo_cap, &doing_cap, title, NULL);
}

static void done_task(const char *title) {
    move_task(&doing, &doing_count, &done, &done_count, &doing_cap, &done_cap, title, NULL);
}

static void remove_any(const char *title) {
    ssize_t idx = find_task(todo, todo_count, title);
    if (idx != -1) { remove_task(&todo, &todo_count, idx); return; }
    idx = find_task(doing, doing_count, title);
    if (idx != -1) { remove_task(&doing, &doing_count, idx); return; }
    idx = find_task(done, done_count, title);
    if (idx != -1) { remove_task(&done, &done_count, idx); }
}

static void find_any(const char *title) {
    ssize_t idx = find_task(todo, todo_count, title);
    if (idx != -1) { printf("todo %s\n", todo[idx].note); return; }
    idx = find_task(doing, doing_count, title);
    if (idx != -1) { printf("doing %s\n", doing[idx].note); return; }
    idx = find_task(done, done_count, title);
    if (idx != -1) { printf("done %s\n", done[idx].note); }
}

static void report(void) {
    for (size_t i = 0; i < todo_count; i++)
        printf("todo %s %s\n", todo[i].title, todo[i].note);
    for (size_t i = 0; i < doing_count; i++)
        printf("doing %s %s\n", doing[i].title, doing[i].note);
    for (size_t i = 0; i < done_count; i++)
        printf("done %s %s\n", done[i].title, done[i].note);
}

static void free_list(Task **list, size_t *count) {
    for (size_t i = 0; i < *count; i++) {
        free((*list)[i].title);
        free((*list)[i].note);
    }
    free(*list);
    *list = NULL;
    *count = 0;
}

static void free_all(void) {
    free_list(&todo, &todo_count);
    free_list(&doing, &doing_count);
    free_list(&done, &done_count);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], title[256], note[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "TODO") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", title, note) == 2)
                todo_add(title, note);
        } else if (strcmp(cmd, "START") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                remove_any(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                find_any(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }

    free(line);
    free_all();
    return 0;
}