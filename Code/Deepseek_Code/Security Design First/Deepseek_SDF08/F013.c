// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct task {
    char *title;
    char *note;
    int list; // 0=todo, 1=doing, 2=done
    struct task *next;
} Task;

Task *todo = NULL, *doing = NULL, *done = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Task *find_task(const char *title) {
    Task *t;
    for (t = todo; t; t = t->next) if (strcmp(t->title, title) == 0) return t;
    for (t = doing; t; t = t->next) if (strcmp(t->title, title) == 0) return t;
    for (t = done; t; t = t->next) if (strcmp(t->title, title) == 0) return t;
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
    if (!t) { fprintf(stderr, "Memory error\n"); exit(1); }
    t->title = strdup(title);
    t->note = strdup(note);
    if (!t->title || !t->note) { fprintf(stderr, "Memory error\n"); exit(1); }
    t->next = *list;
    *list = t;
}

void todo_task(const char *title, const char *note) {
    Task *t = find_task(title);
    if (t) return; // already exists
    add_task(&todo, title, note);
}

void start_task(const char *title) {
    Task *t = find_task(title);
    if (!t) return;
    if (t == todo) {
        remove_from_list(&todo, title);
        add_task(&doing, title, t->note);
    }
    // else if already in doing or done, ignore? The spec: START title moves it to doing.
    // If it's in todo, move; if in doing, do nothing; if in done, maybe ignore.
}

void done_task(const char *title) {
    Task *t = find_task(title);
    if (!t) return;
    if (t == todo) {
        remove_from_list(&todo, title);
        add_task(&done, title, t->note);
    } else if (t == doing) {
        remove_from_list(&doing, title);
        add_task(&done, title, t->note);
    }
    // if already done, ignore
}

void remove_task(const char *title) {
    Task *t = find_task(title);
    if (!t) return;
    if (t == todo) remove_from_list(&todo, title);
    else if (t == doing) remove_from_list(&doing, title);
    else if (t == done) remove_from_list(&done, title);
}

void find_task_print(const char *title) {
    Task *t = find_task(title);
    if (!t) return;
    const char *list_name = "";
    if (t == todo) list_name = "TODO";
    else if (t == doing) list_name = "DOING";
    else if (t == done) list_name = "DONE";
    printf("%s ", list_name);
    for (char *s = t->note; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void report_lists(void) {
    Task *t;
    printf("TODO:");
    for (t = todo; t; t = t->next) { printf(" %s", t->title); }
    printf("\nDOING:");
    for (t = doing; t; t = t->next) { printf(" %s", t->title); }
    printf("\nDONE:");
    for (t = done; t; t = t->next) { printf(" %s", t->title); }
    printf("\n");
}

void free_all(void) {
    Task *t;
    while (todo) { t = todo; todo = todo->next; free(t->title); free(t->note); free(t); }
    while (doing) { t = doing; doing = doing->next; free(t->title); free(t->note); free(t); }
    while (done) { t = done; done = done->next; free(t->title); free(t->note); free(t); }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report_lists(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "TODO") == 0 || strcmp(cmd, "START") == 0 || strcmp(cmd, "DONE") == 0 || strcmp(cmd, "REMOVE") == 0 || strcmp(cmd, "FIND") == 0) {
            char *title = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            if (strcmp(cmd, "TODO") == 0) todo_task(title, rest);
            else if (strcmp(cmd, "START") == 0) start_task(title);
            else if (strcmp(cmd, "DONE") == 0) done_task(title);
            else if (strcmp(cmd, "REMOVE") == 0) remove_task(title);
            else find_task_print(title);
        }
    }
    free_all();
    return 0;
}