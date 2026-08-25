// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct task {
    char *title;
    char *note;
    struct task *next;
};

struct task *todo = NULL;
struct task *doing = NULL;
struct task *done = NULL;

struct task *find_task(char *title, struct task *list) {
    struct task *t = list;
    while (t) {
        if (strcmp(t->title, title) == 0) return t;
        t = t->next;
    }
    return NULL;
}

struct task *find_in_all(char *title) {
    struct task *t = find_task(title, todo);
    if (t) return t;
    t = find_task(title, doing);
    if (t) return t;
    return find_task(title, done);
}

void remove_from_list(char *title, struct task **list) {
    struct task *t = *list, *prev = NULL;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else *list = t->next;
            return;
        }
        prev = t;
        t = t->next;
    }
}

void add_to_list(char *title, char *note, struct task **list) {
    struct task *t = malloc(sizeof(struct task));
    t->title = malloc(strlen(title) + 1);
    strcpy(t->title, title);
    t->note = malloc(strlen(note) + 1);
    strcpy(t->note, note);
    t->next = *list;
    *list = t;
}

void todo_task(char *title, char *note) {
    add_to_list(title, note, &todo);
}

void start_task(char *title) {
    struct task *t = find_task(title, todo);
    if (!t) return;
    remove_from_list(title, &todo);
    t->next = doing;
    doing = t;
}

void done_task(char *title) {
    struct task *t = find_task(title, doing);
    if (!t) return;
    remove_from_list(title, &doing);
    t->next = done;
    done = t;
}

void remove_task(char *title) {
    struct task *t = find_in_all(title);
    if (!t) return;
    if (find_task(title, todo)) remove_from_list(title, &todo);
    else if (find_task(title, doing)) remove_from_list(title, &doing);
    else if (find_task(title, done)) remove_from_list(title, &done);
    free(t->title);
    free(t->note);
    free(t);
}

void find_task_print(char *title) {
    struct task *t = find_in_all(title);
    if (t) {
        char *listname = "todo";
        if (find_task(title, todo)) listname = "todo";
        else if (find_task(title, doing)) listname = "doing";
        else if (find_task(title, done)) listname = "done";
        printf("%s %s\n", listname, t->note);
    }
}

void print_list(struct task *list, char *name) {
    printf("%s:\n", name);
    struct task *t = list;
    while (t) {
        printf("%s %s\n", t->title, t->note);
        t = t->next;
    }
}

void report_all() {
    print_list(todo, "todo");
    print_list(doing, "doing");
    print_list(done, "done");
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char title[256], note[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, title, note);
        if (strcmp(cmd, "TODO") == 0 && n >= 3) {
            todo_task(title, note);
        } else if (strcmp(cmd, "START") == 0 && n >= 2) {
            start_task(title);
        } else if (strcmp(cmd, "DONE") == 0 && n >= 2) {
            done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    return 0;
}