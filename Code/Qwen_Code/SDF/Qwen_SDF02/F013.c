// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Task {
    char *title;
    char *note;
    struct Task *next;
} Task;

Task *todo = NULL, *doing = NULL, *done = NULL;

void add_task(Task **list, const char *title, const char *note) {
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = note ? strdup(note) : NULL;
    t->next = *list;
    *list = t;
}

Task* find_and_remove(Task **list, const char *title) {
    Task *prev = NULL;
    for (Task *t = *list; t; prev = t, t = t->next) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else *list = t->next;
            return t;
        }
    }
    return NULL;
}

Task* find_task(Task *list, const char *title) {
    for (Task *t = list; t; t = t->next)
        if (strcmp(t->title, title) == 0) return t;
    return NULL;
}

void print_text(const char *text) {
    if (!text) return;
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

void print_list(const char *name, Task *list) {
    printf("%s:\n", name);
    for (Task *t = list; t; t = t->next) {
        printf("  %s ", t->title);
        print_text(t->note);
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "TODO") == 0) {
            char title[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", title, &n) != 1) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            add_task(&todo, title, note);
        } else if (strcmp(cmd, "START") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) != 1) continue;
            Task *t = find_and_remove(&todo, title);
            if (t) { t->next = doing; doing = t; }
        } else if (strcmp(cmd, "DONE") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) != 1) continue;
            Task *t = find_and_remove(&doing, title);
            if (t) { t->next = done; done = t; }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) != 1) continue;
            Task *t = find_and_remove(&todo, title);
            if (!t) t = find_and_remove(&doing, title);
            if (!t) t = find_and_remove(&done, title);
            if (t) { free(t->title); free(t->note); free(t); }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) != 1) continue;
            Task *t = find_task(todo, title);
            if (t) { printf("todo "); print_text(t->note); continue; }
            t = find_task(doing, title);
            if (t) { printf("doing "); print_text(t->note); continue; }
            t = find_task(done, title);
            if (t) { printf("done "); print_text(t->note); }
        } else if (strcmp(cmd, "REPORT") == 0) {
            print_list("todo", todo);
            print_list("doing", doing);
            print_list("done", done);
        }
    }
    return 0;
}