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
        Task *t = lists[i];
        while (t) {
            if (strcmp(t->title, title) == 0) {
                *list_ptr = (i == 0 ? todo : (i == 1 ? doing : done));
                return t;
            }
            t = t->next;
        }
    }
    return NULL;
}

void add_task(const char *title, const char *note) {
    Task *dummy;
    if (find_task(title, &dummy)) return;
    Task *t = malloc(sizeof(Task));
    if (!t) return;
    t->title = strdup(title);
    t->note = strdup(note);
    t->next = NULL;
    if (!t->title || !t->note) {
        free(t->title); free(t->note); free(t);
        return;
    }
    if (!todo) todo = t;
    else {
        Task *last = todo;
        while (last->next) last = last->next;
        last->next = t;
    }
}

void move_task(const char *title, Task **src, Task **dst) {
    Task *t = *src, *prev = NULL;
    while (t) {
        if (strcmp(t->title, title) == 0) {
            if (prev) prev->next = t->next;
            else *src = t->next;
            t->next = NULL;
            if (!*dst) *dst = t;
            else {
                Task *last = *dst;
                while (last->next) last = last->next;
                last->next = t;
            }
            return;
        }
        prev = t;
        t = t->next;
    }
}

void start_task(const char *title) {
    Task *dummy;
    Task *t = find_task(title, &dummy);
    if (!t) return;
    // remove from wherever
    if (t == todo) move_task(title, &todo, &doing);
    else if (t == doing) move_task(title, &doing, &done);
    else if (t == done) move_task(title, &done, &doing);
}

void done_task(const char *title) {
    Task *dummy;
    Task *t = find_task(title, &dummy);
    if (!t) return;
    if (t == todo) move_task(title, &todo, &done);
    else if (t == doing) move_task(title, &doing, &done);
    else if (t == done) return;
}

void remove_task(const char *title) {
    Task *dummy;
    Task *t = find_task(title, &dummy);
    if (!t) return;
    // remove from its list
    if (t == todo) move_task(title, &todo, NULL);
    else if (t == doing) move_task(title, &doing, NULL);
    else if (t == done) move_task(title, &done, NULL);
}

void find_task_print(const char *title) {
    Task *dummy;
    Task *t = find_task(title, &dummy);
    if (!t) return;
    char *listname = "todo";
    if (t == doing) listname = "doing";
    else if (t == done) listname = "done";
    printf("%s %s\n", listname, t->note);
}

void report_all(void) {
    void print_list(const char *name, Task *list) {
        printf("%s\n", name);
        Task *t = list;
        while (t) {
            printf("%s %s\n", t->title, t->note);
            t = t->next;
        }
    }
    print_list("todo", todo);
    print_list("doing", doing);
    print_list("done", done);
}

void free_list(Task *list) {
    Task *t = list;
    while (t) {
        Task *next = t->next;
        free(t->title);
        free(t->note);
        free(t);
        t = next;
    }
}

void free_all(void) {
    free_list(todo);
    free_list(doing);
    free_list(done);
    todo = doing = done = NULL;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], title[256], note[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "TODO") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", title, note) == 2)
                add_task(title, note);
        } else if (strcmp(cmd, "START") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                start_task(title);
        } else if (strcmp(cmd, "DONE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                done_task(title);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                remove_task(title);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", title) == 1)
                find_task_print(title);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}